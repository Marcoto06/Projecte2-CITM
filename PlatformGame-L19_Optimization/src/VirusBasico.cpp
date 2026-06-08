#include "VirusBasico.h"
#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Scene.h"
#include "Physics.h"
#include "Log.h"
#include "tracy/Tracy.hpp"
#include "EntityManager.h"
#include <cstdlib>
#include "CelulaBasica.h"
#include <cmath>

VirusBasico::VirusBasico() : Entity(EntityType::VIRUS_BASICO)
{
	name = "VirusBasico";
}

VirusBasico::~VirusBasico()
{
}

bool VirusBasico::Awake()
{
	return true;
}

bool VirusBasico::Start()
{
	std::unordered_map<int, std::string> aliases =
	{
		{0,"idle"},
		{32,"attack1"},
		{64,"attack2"},
		{96,"parasite"},
		{128,"hurt"},
		{160,"groundHit"},
		{167,"stun"},
		{192,"death"},
		{224,"revive"}
	};

	anims.LoadFromTSX("Assets/Textures/Characters/Atlas_Virus_Basico.tsx", aliases);
	anims.SetCurrent("idle");

	anims.Func_SetAnimationLoop("attack1", false);
	anims.Func_SetAnimationLoop("attack2", false);
	anims.Func_SetAnimationLoop("parasitar", false);
	anims.Func_SetAnimationLoop("hurt", false);
	anims.Func_SetAnimationLoop("death", false);
	anims.Func_SetAnimationLoop("groundHit", false);
	anims.Func_SetAnimationLoop("revive", false);

	texture = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Atlas_Virus_Basico.png");

	//audios
	idleFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx virus/Virus_IDLE.wav");
	attack1FxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx virus/Virus_Ataque1.wav");
	attack2FxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx virus/Virus_Ataque2.wav");
	parasiteFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx virus/Virus_AtaqueParasitar.wav");
	groundHitFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx virus/Virus_Hurt.wav");
	deathFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx virus/Virus_Morir.wav");
	reviveFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx virus/Virus_Revivir.wav");

	texW = 256;
	texH = 256;

	pbody = Engine::GetInstance().physics->CreateCircle(
		(int)position.getX() + texW / 2,
		(int)position.getY() + texH / 2,
		texW / 2,
		bodyType::DYNAMIC
	);

	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;

	b2Body_SetGravityScale(pbody->body, 0.0f);

	player = Engine::GetInstance().scene->player.get();

	return true;
}

bool VirusBasico::Update(float dt)
{
	ZoneScoped;

	if (currentState == VIRUS_STATE::IDLE || currentState == VIRUS_STATE::MOVING)
	{
		
		if (idleChannel == -1)
		{

			idleChannel = Engine::GetInstance().audio->PlayFx(idleFxId);
		}
	}
	else
	{
		if (idleChannel != -1)
		{
			idleChannel = -1;
		}
	}

	GetPhysicsValues();

	if (!canAttack && attackCooldownTimer.ReadMSec() >= attackCooldownMs)
	{
		canAttack = true;
	}

	if (isStunned && currentState == VIRUS_STATE::STUNED)
	{
		b2Body_SetGravityScale(pbody->body, 100.0f);
		Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, 0, 0);
	}
	else if (
		currentState != VIRUS_STATE::ATTACKING &&
		currentState != VIRUS_STATE::TEMP_DEATH &&
		currentState != VIRUS_STATE::REVIVING
		)
	{
		isPlayerDetected = IsPlayerDetected();
		CelulaBasica* nearbyCell = FindNearestCell(550.0f);

		if (nearbyCell != nullptr && canAttack)
		{
			Vector2D cellPos = nearbyCell->GetPosition();

			currentAttackTarget = Vector2D(
				cellPos.getX() + 128.0f,
				cellPos.getY() + 128.0f
			);

			int roll = std::rand() % 100;

			attackingCell = true;
			currentState = VIRUS_STATE::ATTACKING;
			hasSpawnedProjectile = false;
			canAttack = false;
			attackTimer.Start();

			if (roll < 45)
			{
				currentAttack = 3;
				anims.SetCurrent("parasite");
			}
			else
			{
				currentAttack = 2;
				anims.SetCurrent("attack2");
			}

			return true;
		}
		if (isPlayerDetected)
		{
			Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
			Vector2D myPos = GetPosition();

			float virusCenterX = myPos.getX() + texW * 0.5f;
			float virusCenterY = myPos.getY() + texH * 0.5f;

			float playerCenterX = playerPos.getX() + 32.0f;
			float playerCenterY = playerPos.getY() + 32.0f;

			float dx = playerCenterX - virusCenterX;
			float dy = playerCenterY - virusCenterY;

			float distance = std::sqrt(dx * dx + dy * dy);

			if (canAttack && distance <= 380.0f)
			{
				currentState = VIRUS_STATE::ATTACKING;
				hasSpawnedProjectile = false;
				canAttack = false;
				attackTimer.Start();

				currentAttack = 1;
				anims.SetCurrent("attack1");
			}
			else if (canAttack && distance <= 500.0f)
			{
				currentState = VIRUS_STATE::ATTACKING;
				hasSpawnedProjectile = false;
				canAttack = false;
				attackTimer.Start();

				currentAttack = 2;
				anims.SetCurrent("attack2");
			}
			else
			{
				currentState = VIRUS_STATE::MOVING;
			}
		}
		else
		{
			currentState = VIRUS_STATE::IDLE;
		}
	}

	Func_EnemyStates(dt);
	ApplyPhysics();
	Draw(dt);

	return true;
}

void VirusBasico::GetPhysicsValues()
{
	velocity = b2Vec2_zero;
}

void VirusBasico::Func_EnemyStates(float dt)
{
	switch (currentState)
	{
	case VIRUS_STATE::IDLE:
		velocity = b2Vec2_zero;
		anims.SetCurrent("idle");
		break;

	case VIRUS_STATE::MOVING:
		anims.SetCurrent("idle");
		Move();
		break;

	case VIRUS_STATE::ATTACKING:
	{
		velocity = b2Vec2_zero;

		if (currentAttack == 1)
		{
			anims.SetCurrent("attack1");
		}
		else
		{
			anims.SetCurrent("attack2");
		}

		if (!hasSpawnedProjectile)
		{
			hasSpawnedProjectile = true;

			if (currentAttack == 1)
			{
				Engine::GetInstance().audio->PlayFx(attack1FxId);
				SpawnWaveProjectiles();
			}
			else if (currentAttack == 2)
			{
				Engine::GetInstance().audio->PlayFx(attack2FxId);
				if (attackingCell)
					SpawnBigProjectileToTarget(currentAttackTarget);
				else
					SpawnBigProjectile();
			}
			else if (currentAttack == 3)
			{
				Engine::GetInstance().audio->PlayFx(parasiteFxId);
				SpawnParasiteProjectileToTarget(currentAttackTarget);
			}
		}

		if (anims.Func_HasCurrentAnimationFinished())
		{
			attackingCell = false;
			attackCooldownTimer.Start();
			currentState = VIRUS_STATE::MOVING;
		}

		break;
	}

	case VIRUS_STATE::STUNED:
		velocity = b2Vec2_zero;

		if (!groundHitPlayed)
		{
			if (anims.Func_HasCurrentAnimationFinished())
			{
				groundHitPlayed = true;
				anims.SetCurrent("stun");
			}
		}
		else
		{
			anims.SetCurrent("stun");
		}

		if (timer_01.ReadMSec() >= stuntimer * 1000.0f)
		{
			isStunned = false;
			groundHitPlayed = false;

			b2Body_SetGravityScale(pbody->body, 0.0f);

			currentState = VIRUS_STATE::IDLE;
		}

		break;

	case VIRUS_STATE::TEMP_DEATH:
		velocity = b2Vec2_zero;
		anims.SetCurrent("death");

		if (reviveTimer.ReadMSec() >= reviveTime * 1000.0f)
		{
			anims.SetCurrent("revive");
			currentState = VIRUS_STATE::REVIVING;
			Engine::GetInstance().audio->PlayFx(reviveFxId);
		}

		break;

	case VIRUS_STATE::REVIVING:
		velocity = b2Vec2_zero;
		anims.SetCurrent("revive");

		if (anims.Func_HasCurrentAnimationFinished())
		{
			isStunned = false;
			groundHitPlayed = false;
			currentState = VIRUS_STATE::IDLE;
		}

		break;
	}
}

void VirusBasico::Move()
{
	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
	Vector2D myPos = GetPosition();

	float dx = playerPos.getX() - myPos.getX();
	float dy = playerPos.getY() - myPos.getY();

	float distance = std::sqrt(dx * dx + dy * dy);

	if (distance <= 1.0f)
	{
		velocity = b2Vec2_zero;
		return;
	}

	dx /= distance;
	dy /= distance;

	float minDistance = 280.0f;
	float maxDistance = 500.0f;

	if (distance > maxDistance)
	{
		velocity.x = dx * speed * 1.4f;
		velocity.y = dy * speed * 1.4f;
	}
	else if (distance < minDistance)
	{
		velocity.x = -dx * speed * 1.6f;
		velocity.y = -dy * speed * 1.6f;
	}
	else
	{
		velocity = b2Vec2_zero;
	}

	if (dx > 0.0f)
	{
		isFacingRight = true;
	}
	else if (dx < 0.0f)
	{
		isFacingRight = false;
	}
}

bool VirusBasico::IsPlayerDetected() const
{
	if (Engine::GetInstance().scene->player->IsGodMode())
		return false;

	Vector2D playerPosition = Engine::GetInstance().scene->GetPlayerPosition();
	Vector2D enemyPosition = const_cast<VirusBasico*>(this)->GetPosition();

	float distanceX = playerPosition.getX() - enemyPosition.getX();
	float distanceY = playerPosition.getY() - enemyPosition.getY();
	float squaredDistance = distanceX * distanceX + distanceY * distanceY;

	return squaredDistance <= detectionRange * detectionRange;
}

void VirusBasico::ApplyPhysics()
{
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void VirusBasico::Draw(float dt)
{
	anims.Update(dt);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	int x, y;
	pbody->GetPosition(x, y);

	position.setX((float)x);
	position.setY((float)y);

	int frameW = animFrame.w;
	int frameH = animFrame.h;

	int drawX = x - frameW / 2;
	int drawY = y - frameH / 2;

	Engine::GetInstance().render->DrawTexture(
		texture,
		drawX,
		drawY,
		&animFrame,
		1.0f,
		0.0,
		frameW / 2,
		frameH / 2,
		isFacingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE,
		1.0f
	);
}

bool VirusBasico::CleanUp()
{
	if (texture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(texture);
		texture = nullptr;
	}

	if (pbody != nullptr)
	{
		Engine::GetInstance().physics->DeletePhysBody(pbody);
		pbody = nullptr;
	}

	return true;
}

bool VirusBasico::Destroy()
{
	active = false;
	pendingToDelete = true;
	return true;
}

void VirusBasico::SetPosition(Vector2D pos)
{
	if (pbody != nullptr)
	{
		pbody->SetPosition((int)pos.getX(), (int)pos.getY());
	}
}

Vector2D VirusBasico::GetPosition()
{
	int x, y;
	pbody->GetPosition(x, y);

	return Vector2D((float)x - texW / 2, (float)y - texH / 2);
}

bool VirusBasico::IsEnemyStunned()
{
	return currentState == VIRUS_STATE::STUNED || currentState == VIRUS_STATE::TEMP_DEATH;
}

CelulaBasica* VirusBasico::FindNearestCell(float range)
{
	CelulaBasica* nearestCell = nullptr;
	float bestDistanceSq = range * range;

	Vector2D myPos = GetPosition();
	float myCenterX = myPos.getX() + texW * 0.5f;
	float myCenterY = myPos.getY() + texH * 0.5f;

	for (const auto& entity : Engine::GetInstance().entityManager->entities)
	{
		if (entity == nullptr || !entity->active)
			continue;

		if (entity->type != EntityType::CELULA_BASICA)
			continue;

		CelulaBasica* cell = dynamic_cast<CelulaBasica*>(entity.get());

		if (cell == nullptr || cell->IsParasitized())
			continue;

		Vector2D cellPos = cell->GetPosition();

		float dx = cellPos.getX() - myCenterX;
		float dy = cellPos.getY() - myCenterY;
		float distSq = dx * dx + dy * dy;

		if (distSq < bestDistanceSq)
		{
			bestDistanceSq = distSq;
			nearestCell = cell;
		}
	}

	return nearestCell;
}

void VirusBasico::SpawnBigProjectile()
{

	std::shared_ptr<ProyectilesVirusBasico> projectile = std::make_shared<ProyectilesVirusBasico>();

	Vector2D virusPos = GetPosition();
	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();

	float spawnOffsetX = isFacingRight ? 90.0f : -90.0f;

	Vector2D spawnPos(
		virusPos.getX() + texW / 2 + spawnOffsetX,
		virusPos.getY() + texH / 2
	);

	Vector2D direction(
		playerPos.getX() - spawnPos.getX(),
		playerPos.getY() - spawnPos.getY()
	);

	projectile->SetProjectileType(2);
	projectile->SetSpawnPosition(spawnPos);
	projectile->SetDirection(direction);
	projectile->Start();

	Engine::GetInstance().entityManager->AddEntity(projectile);
}

void VirusBasico::SpawnBigProjectileToTarget(Vector2D targetPos)
{
	std::shared_ptr<ProyectilesVirusBasico> projectile = std::make_shared<ProyectilesVirusBasico>();

	Vector2D virusPos = GetPosition();

	float spawnOffsetX = isFacingRight ? 90.0f : -90.0f;

	Vector2D spawnPos(
		virusPos.getX() + texW / 2 + spawnOffsetX,
		virusPos.getY() + texH / 2
	);

	Vector2D direction(
		targetPos.getX() - spawnPos.getX(),
		targetPos.getY() - spawnPos.getY()
	);

	projectile->SetProjectileType(2);
	projectile->SetSpawnPosition(spawnPos);
	projectile->SetDirection(direction);
	projectile->Start();

	Engine::GetInstance().entityManager->AddEntity(projectile);
}

void VirusBasico::SpawnParasiteProjectileToTarget(Vector2D targetPos)
{
	std::shared_ptr<ProyectilesVirusBasico> projectile = std::make_shared<ProyectilesVirusBasico>();

	Vector2D virusPos = GetPosition();

	float spawnOffsetX = isFacingRight ? 90.0f : -90.0f;

	Vector2D spawnPos(
		virusPos.getX() + texW / 2 + spawnOffsetX,
		virusPos.getY() + texH / 2
	);

	Vector2D direction(
		targetPos.getX() - spawnPos.getX(),
		targetPos.getY() - spawnPos.getY()
	);

	projectile->SetProjectileType(3);
	projectile->SetSpawnPosition(spawnPos);
	projectile->SetDirection(direction);
	projectile->Start();

	Engine::GetInstance().entityManager->AddEntity(projectile);
}

void VirusBasico::SpawnParasiteProjectile()
{
	std::shared_ptr<ProyectilesVirusBasico> projectile = std::make_shared<ProyectilesVirusBasico>();

	Vector2D virusPos = GetPosition();

	float spawnOffsetX = isFacingRight ? 90.0f : -90.0f;

	Vector2D spawnPos(
		virusPos.getX() + texW / 2 + spawnOffsetX,
		virusPos.getY() + texH / 2
	);

	Vector2D direction(
		isFacingRight ? 1.0f : -1.0f,
		0.0f
	);

	projectile->SetProjectileType(3);
	projectile->SetSpawnPosition(spawnPos);
	projectile->SetDirection(direction);
	projectile->Start();

	Engine::GetInstance().entityManager->AddEntity(projectile);
}

void VirusBasico::SpawnWaveProjectiles()
{
	const int projectileCount = 8;
	const float PI = 3.14159265f;

	Vector2D virusPos = GetPosition();

	Vector2D spawnPos(
		virusPos.getX() + texW / 2,
		virusPos.getY() + texH / 2
	);

	for (int i = 0; i < projectileCount; ++i)
	{
		float angle = (2.0f * PI / projectileCount) * i;

		Vector2D direction(
			std::cos(angle),
			std::sin(angle)
		);

		std::shared_ptr<ProyectilesVirusBasico> projectile =
			std::make_shared<ProyectilesVirusBasico>();

		projectile->SetProjectileType(1);
		projectile->SetSpawnPosition(spawnPos);
		projectile->SetDirection(direction);
		projectile->Start();

		Engine::GetInstance().entityManager->AddEntity(projectile);
	}
}

void VirusBasico::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::SYRINGE:
		if (!isStunned)
		{
			timer_01.Start();
			isStunned = true;
			groundHitPlayed = false;
			anims.SetCurrent("groundHit");
			currentState = VIRUS_STATE::STUNED;
			Engine::GetInstance().audio->PlayFx(groundHitFxId);
		}
		break;

	case ColliderType::SUCK_ZONE:
		if (currentState == VIRUS_STATE::STUNED)
		{
			groundHitPlayed = false;
			isStunned = true;

			velocity = b2Vec2_zero;
			Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);

			b2Body_SetGravityScale(pbody->body, 0.0f);

			anims.SetCurrent("death");
			reviveTimer.Start();

			currentState = VIRUS_STATE::TEMP_DEATH;
			Engine::GetInstance().audio->PlayFx(deathFxId);
		}
		break;

	case ColliderType::PLATFORM:
		if (isStunned && isFallingToGround)
		{
			isFallingToGround = false;
			hasLandedStunned = true;

			velocity = b2Vec2_zero;
			Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);

			timer_01.Start();
			anims.SetCurrent("groundHit");
			Engine::GetInstance().audio->PlayFx(groundHitFxId);
		}
		break;
	}
}

void VirusBasico::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
}
