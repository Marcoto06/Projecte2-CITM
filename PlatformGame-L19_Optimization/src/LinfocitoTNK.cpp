#include <cmath>
#include "LinfocitoTNK.h"
#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"
#include "Map.h"
#include "tracy/Tracy.hpp"

LinfocitoTNK::LinfocitoTNK() : Entity(EntityType::LINFOCITO_TNK)
{
	name = "LinfocitoTNK";
}

LinfocitoTNK::~LinfocitoTNK() {

}

bool LinfocitoTNK::Awake() {
	return true;
}

bool LinfocitoTNK::Start() {

	std::unordered_map<int, std::string> aliases = {
	{0, "idle"},
	{26, "walk"},
	{52, "hurt"},
	{78, "caer"},
	{85, "stun"},
	{104, "death"},
	{130, "cargarRodar"},
	{152, "rodar"},
	{156, "chocar1"},
	{182, "chocar2"},
	{208, "cargarSalto"},
	{234, "cargarSalto2"},
	{260, "salto"},
	{266, "topSalto"},
	{268, "bajadaSalto"}
	};

	anims.LoadFromTSX("Assets/Textures/Characters/Atlas_Linfocito-T-NK.tsx", aliases);
	anims.SetCurrent("idle");

	anims.Func_SetAnimationLoop("death", false);
	anims.Func_SetAnimationLoop("cargarRodar", false);
	anims.Func_SetAnimationLoop("chocar1", false);
	anims.Func_SetAnimationLoop("chocar2", false);
	anims.Func_SetAnimationLoop("cargarSalto", false);
	anims.Func_SetAnimationLoop("cargarSalto2", false);
	anims.Func_SetAnimationLoop("salto", false);
	anims.Func_SetAnimationLoop("topSalto", false);
	anims.Func_SetAnimationLoop("bajadaSalto", false);
	//audio fx
	walkFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx linfocito TNK/tnk walk.wav");
	cargarRodarFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx linfocito TNK/tnk cargar rodar.wav");
	rodarFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx linfocito TNK/tnk rodar.wav");
	cargarSaltoFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx linfocito TNK/tnk cargar salto.wav");
	aterrizarFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx linfocito TNK/tnk aterrizar.wav");
	hurtFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx linfocito TNK/tnk hurt.wav");
	deathFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx linfocito TNK/tnk morir.wav");

	texture = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Atlas_Linfocito-T-NK.png");

	texW = 188;
	texH = 150;
	pbody = Engine::GetInstance().physics->CreateRectangle(
		(int)position.getX() + texW / 2,
		(int)position.getY() + texH / 2,
		texW,
		texH,
		bodyType::DYNAMIC
	);
	pbody->SetFixedRotation(true);

	pbody->listener = this;

	pbody->ctype = ColliderType::ENEMY;

	pathfinding = std::make_shared<Pathfinding>();
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map->WorldToMap((int)pos.getX(), (int)pos.getY() + 1);
	pathfinding->ResetPath(tilePos);
	player = Engine::GetInstance().scene->player.get();

	return true;
}

bool LinfocitoTNK::Update(float dt)
{
	ZoneScoped;

	if (!canBeHit && hitCooldownTimer.ReadMSec() > 500.0f) 
	{
		canBeHit = true;
	}

	GetPhysicsValues();

	if (!canDamagePlayer && damageTimer.ReadMSec() >= damageCooldown * 1000.0f)
	{
		canDamagePlayer = true;
	}

	if (!canStartAttack && attackCooldownTimer.ReadMSec() >= attackCooldownTime)
	{
		canStartAttack = true;
	}

	if (isTouchingPlayer && touchingPlayer != nullptr && canDamagePlayer)
	{
		if (!touchingPlayer->IsGodMode())
		{
			touchingPlayer->playerCurrentHp -= damage;

			if (touchingPlayer->playerCurrentHp <= 0)
			{
				touchingPlayer->playerCurrentHp = 0;
			}

			canDamagePlayer = false;
			damageTimer.Start();
		}
	}

	if (!isStunned &&
		currentEState != ENEMYSTATES::ATTACK &&
		currentEState != ENEMYSTATES::DEATH)
	{
		isPlayerDetected = IsPlayerDetected();

		if (isPlayerDetected)
		{
			currentEState = ENEMYSTATES::CHASING;
		}
		else
		{
			currentEState = ENEMYSTATES::WALKING;
		}
	}

	Vector2D currentPos = GetPosition();
	bool isVisible = Engine::GetInstance().render->IsOnScreenWorldRect(currentPos.getX(), currentPos.getY(), texW, texH, 150);

	if (isVisible && currentEState == ENEMYSTATES::CHASING)
	{
		pathfindingFrameCount++;

		if (pathfindingFrameCount >= pathfindingUpdateRate)
		{
			PerformPathfinding();
			pathfindingFrameCount = 0;
		}
	}
	else
	{
		pathfindingFrameCount = 0;
	}

	Func_EnemyStates(dt);
	ApplyPhysics();
	Draw(dt);

	return true;
}

void LinfocitoTNK::PerformPathfinding()
{
	Map* map = Engine::GetInstance().map.get();

	Vector2D enemyPosition = GetPosition();
	Vector2D playerPosition = Engine::GetInstance().scene->GetPlayerPosition();

	Vector2D originTile = map->WorldToMap(
		(int)(enemyPosition.getX() + (texW * 0.5f)),
		(int)(enemyPosition.getY() + (texH * 0.5f))
	);

	Vector2D destinationTile = map->WorldToMap(
		(int)(playerPosition.getX() + 16),
		(int)(playerPosition.getY() + 16)
	);

	pathfinding->ResetPath(originTile);

	while (pathfinding->CanPropagateAStar(destinationTile))
	{
		pathfinding->PropagateAStar(SQUARED, destinationTile);
	}
}

void LinfocitoTNK::GetPhysicsValues() {
	velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
	velocity = { 0, velocity.y };
}

void LinfocitoTNK::Func_EnemyStates(float dt)
{
	if (currentEState == ENEMYSTATES::CHASING && IsPlayerDetected() && walkAudioTimer.ReadMSec() >= 3000.0f)
	{
		Engine::GetInstance().audio->PlayFx(walkFxId);
		walkAudioTimer.Start();
	}
	switch (currentEState)
	{
	case LinfocitoTNK::ENEMYSTATES::WALKING:
		anims.SetCurrent("idle");
		Move();
		break;

	case ENEMYSTATES::CHASING:
	{
		Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
		Vector2D myPos = GetPosition();

		float dx = playerPos.getX() - myPos.getX();
		float dy = playerPos.getY() - myPos.getY();

		if (!canStartAttack)
		{
			anims.SetCurrent("walk");
			Move();
			break;
		}

		if (std::abs(dx) < 230.0f && std::abs(dy) < 90.0f)
		{
			isFacingRight = dx > 0.0f;
			attackType = 1;
			attackPhase = 0;
			anims.SetCurrent("cargarRodar");
			currentEState = ENEMYSTATES::ATTACK;
			soundCargarRodarPlayed = false;
			soundRodarPlayed = false;
		}
		else if (std::abs(dx) < 340.0f && std::abs(dy) < 140.0f)
		{
			isFacingRight = dx > 0.0f;
			attackType = 2;
			attackPhase = 0;
			jumpTargetX = (int)(playerPos.getX() + texW / 2);

			if (previousAttackType == 1)
				anims.SetCurrent("cargarSalto2");
			else
				anims.SetCurrent("cargarSalto");

			currentEState = ENEMYSTATES::ATTACK;

			attackTimer.Start();
			soundCargarSaltoPlayed = false;
		}
		else
		{
			anims.SetCurrent("walk");
			Move();
		}

		break;
	}

	case ENEMYSTATES::ATTACK:
	{
		if (attackType == 1)
		{
			if (attackPhase == 0)
			{
				velocity.x = 0.0f;
				anims.SetCurrent("cargarRodar");

				if (!soundCargarRodarPlayed)
				{
					Engine::GetInstance().audio->PlayFx(cargarRodarFxId);
					soundCargarRodarPlayed = true;
				}

				if (anims.Func_HasCurrentAnimationFinished())
				{
					attackPhase = 1;
					anims.SetCurrent("rodar");
					attackTimer.Start();
				}
			}
			else if (attackPhase == 1)
			{
				velocity.x = isFacingRight ? rollSpeed : -rollSpeed;

				if (!soundRodarPlayed)
				{
					Engine::GetInstance().audio->PlayFx(rodarFxId);
					soundRodarPlayed = true;
				}

				if (attackTimer.ReadMSec() >= rollAttackTime)
				{
					attackPhase = 2;
					velocity.x = 0.0f;
					if (previousAttackType == 2)
						anims.SetCurrent("chocar2");
					else
						anims.SetCurrent("chocar1");
				}
			}
			else if (attackPhase == 2)
			{
				velocity.x = 0.0f;

				if (anims.Func_HasCurrentAnimationFinished())
				{
					previousAttackType = 1;
					attackType = 0;
					attackPhase = 0;
					canStartAttack = false;
					attackCooldownTimer.Start();
					currentEState = ENEMYSTATES::CHASING;
				}
			}
		}
		else if (attackType == 2)
		{
			if (attackPhase == 0)
			{
				velocity.x = 0.0f;
				if (previousAttackType == 1)
					anims.SetCurrent("cargarSalto2");
				else
					anims.SetCurrent("cargarSalto");

				// adjust timer audio
				if (!soundCargarSaltoPlayed && attackTimer.ReadMSec() >= 600.0f)
				{
					Engine::GetInstance().audio->PlayFx(cargarSaltoFxId);
					soundCargarSaltoPlayed = true;
				}

				if (anims.Func_HasCurrentAnimationFinished())
				{
					attackPhase = 1;
					anims.SetCurrent("salto");
					attackTimer.Start();
				}
			} 
			else if (attackPhase == 1)
			{
				int x, y;
				pbody->GetPosition(x, y);

				float diffX = (float)jumpTargetX - (float)x;

				if (std::abs(diffX) > 8.0f)
				{
					velocity.x = diffX > 0.0f ? jumpAttackSpeedX : -jumpAttackSpeedX;
				}
				else
				{
					velocity.x = 0.0f;
				}
				anims.SetCurrent("salto");
				velocity.y = -6.0f;

				if (attackTimer.ReadMSec() >= jumpUpTime)
				{
					attackPhase = 2;
					anims.SetCurrent("topSalto");
					attackTimer.Start();
				}
			}
			else if (attackPhase == 2)
			{
				int x, y;
				pbody->GetPosition(x, y);

				float diffX = (float)jumpTargetX - (float)x;

				if (std::abs(diffX) > 8.0f)
				{
					velocity.x = diffX > 0.0f ? jumpAttackSpeedX : -jumpAttackSpeedX;
				}
				else
				{
					velocity.x = 0.0f;
				}
				anims.SetCurrent("bajadaSalto");
				velocity.y = 6.0f;


				//adjust timer audio
				if (soundCargarSaltoPlayed && attackTimer.ReadMSec() >= 100)
				{
					Engine::GetInstance().audio->PlayFx(aterrizarFxId);
					soundCargarSaltoPlayed = false; 
				}

				if (attackTimer.ReadMSec() >= jumpDownTime)
				{
					
					previousAttackType = 2;
					attackType = 0;
					attackPhase = 0;
					velocity.x = 0.0f;
					canStartAttack = false;
					attackCooldownTimer.Start();
					currentEState = ENEMYSTATES::CHASING;
				}
			}
		}
	}

		break;

	case LinfocitoTNK::ENEMYSTATES::STUNED:
		anims.SetCurrent("stun");

		if (isBeingSucked)
		{
			if (!player->isAdrenaline)
			{
				if (suckTimer.ReadMSec() >= 3000.0f)
				{
					Engine::GetInstance().audio->PlayFx(deathFxId);
					currentEState = ENEMYSTATES::DEATH;
					return;


				}
			}
			else
			{
				if (suckTimer.ReadMSec() >= absorbTime * 1000.0f)
				{
					Engine::GetInstance().audio->PlayFx(deathFxId);
					currentEState = ENEMYSTATES::DEATH;
					return;


				}
			}
		}
		else
		{
			if (timer_01.ReadMSec() > stuntimer * 1000.0f)
			{
				currentEState = ENEMYSTATES::WALKING;
				isStunned = false;
			}
		}
		break;

	case LinfocitoTNK::ENEMYSTATES::DEATH:

		anims.SetCurrent("death");
		if (anims.Func_HasCurrentAnimationFinished())
		{
			Destroy(attackingPlayer);
			return;
		}
		break;
	default:
		break;
	}

};

void LinfocitoTNK::Move()
{
	velocity.x = 0.0f;

	switch (currentEState)
	{
	case ENEMYSTATES::WALKING:
		break;

	case ENEMYSTATES::CHASING:
	{
		Vector2D nextPathTile = GetNextPathTile();

		if (nextPathTile.getX() < 0 || nextPathTile.getY() < 0)
		{
			break;
		}

		Map* map = Engine::GetInstance().map.get();
		Vector2D nextTileWorldPosition = map->MapToWorld((int)nextPathTile.getX(), (int)nextPathTile.getY());

		float targetX = nextTileWorldPosition.getX() + (map->GetTileWidth() * 0.5f);
		float currentX = GetPosition().getX() + (texW * 0.5f);

		const float horizontalTolerance = 4.0f;
		float deltaX = targetX - currentX;

		if (deltaX > horizontalTolerance)
		{
			velocity.x = speed;
			isFacingRight = true;
		}
		else if (deltaX < -horizontalTolerance)
		{
			velocity.x = -speed;
			isFacingRight = false;
		}

		break;
	}

	case ENEMYSTATES::STUNED:
		break;

	default:
		break;
	}
}

bool LinfocitoTNK::IsPlayerDetected() const
{
	if (Engine::GetInstance().scene->player->IsGodMode())
		return false;

	Vector2D playerPosition = Engine::GetInstance().scene->GetPlayerPosition();
	Vector2D enemyPosition = const_cast<LinfocitoTNK*>(this)->GetPosition();

	float distanceX = playerPosition.getX() - enemyPosition.getX();
	float distanceY = playerPosition.getY() - enemyPosition.getY();
	float squaredDistance = (distanceX * distanceX) + (distanceY * distanceY);

	return squaredDistance <= (detectionRange * detectionRange);
}

Vector2D LinfocitoTNK::GetNextPathTile() const
{
	const std::list<Vector2D>& pathTiles = pathfinding->GetPathTiles();

	if (pathTiles.size() <= 1)
	{
		return Vector2D(-1, -1);
	}

	auto nextTileIt = pathTiles.rbegin();
	++nextTileIt; 
	return *nextTileIt;
}

void LinfocitoTNK::ApplyPhysics() {

	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void LinfocitoTNK::Draw(float dt)
{
	anims.Update(dt);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);

	int frameW = animFrame.w;
	int frameH = animFrame.h;

	int drawX = x - (frameW / 2);
	int drawY = y - (frameH / 2) - 150; // Adjusted the enemy 's vertical position to align with the sprite

	if (isFacingRight)
	{
		Engine::GetInstance().render->DrawTexture(
			texture,
			drawX,
			drawY,
			&animFrame,
			1.0f,
			0.0,
			(frameW / 2),
			(frameH / 2),
			SDL_FLIP_NONE,
			1.0f
		);
	}
	else
	{
		Engine::GetInstance().render->DrawTexture(
			texture,
			drawX,
			drawY,
			&animFrame,
			1.0f,
			0.0,
			(frameW / 2),
			(frameH / 2),
			SDL_FLIP_HORIZONTAL,
			1.0f
		);
	}
}

bool LinfocitoTNK::CleanUp()
{
	LOG("Cleanup enemy");
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

bool LinfocitoTNK::Destroy()
{
	LOG("Destroying Enemy");
	active = false;

	if (this->tiledId != -1) {
		auto& deadList = Engine::GetInstance().scene->destroyedEntitiesIds;

		if (std::find(deadList.begin(), deadList.end(), this->tiledId) == deadList.end()) {
			deadList.push_back(this->tiledId);
		}
	}

	pendingToDelete = true;
	return true;
}

bool LinfocitoTNK::Destroy(Player* pplayer) 
{
	player->isAdrenaline = true;
	player->effectAnims.SetCurrent("lifeUp");
	return Destroy();
}

void LinfocitoTNK::SetPosition(Vector2D pos) {
	pbody->SetPosition((int)(pos.getX()), (int)(pos.getY()));
}

Vector2D LinfocitoTNK::GetPosition() {
	int x, y;
	pbody->GetPosition(x, y);
	
	return Vector2D((float)x - texW / 2, (float)y - texH / 2);
}

bool LinfocitoTNK::IsEnemyStunned() {

	if (currentEState == ENEMYSTATES::STUNED or currentEState == ENEMYSTATES::DEATH)
	{
		return true;
	}
	else return false;
}

void LinfocitoTNK::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
	{
		touchingPlayer = (Player*)physB->listener;
		isTouchingPlayer = touchingPlayer != nullptr;
		break;
	
	}
	case ColliderType::SYRINGE:
		if (!isStunned && canBeHit) 
		{
			syringeHits++;
			canBeHit = false;        
			hitCooldownTimer.Start(); 

			if (syringeHits >= 5)
			{
				Engine::GetInstance().audio->PlayFx(hurtFxId);
				timer_01.Start();
				currentEState = ENEMYSTATES::STUNED;
				isStunned = true;
				syringeHits = 0;

				if (player->isBerserker)
				{
					player->RestoreHealthB();
				}
			}
		}
		break;
	case ColliderType::SUCK_ZONE:
		if (isStunned && !isBeingSucked) {
			isBeingSucked = true;
			suckTimer.Start();

			attackingPlayer = (Player*)physB->listener;
		}
		break;
	}
}

void LinfocitoTNK::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		isTouchingPlayer = false;
		touchingPlayer = nullptr;
		break;

	case ColliderType::SUCK_ZONE:
		isBeingSucked = false;
		break;
	}
}
