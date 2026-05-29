#include "CelulaBasica.h"
#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Scene.h"
#include "Physics.h"
#include "Log.h"
#include "tracy/Tracy.hpp"
#include <cmath>
#include <cstdlib>

CelulaBasica::CelulaBasica() : Entity(EntityType::CELULA_BASICA)
{
	name = "CelulaBasica";
}

CelulaBasica::~CelulaBasica()
{
}

bool CelulaBasica::Awake()
{
	return true;
}

bool CelulaBasica::Start()
{
	LoadCellData();

	pbody = Engine::GetInstance().physics->CreateRectangle(
		(int)position.getX() + texW / 2,
		(int)position.getY() + texH / 2,
		texW,
		texH,
		bodyType::DYNAMIC
	);

	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;
	pbody->SetFixedRotation(true);

	player = Engine::GetInstance().scene->player.get();

	moveTimer.Start();
	idleTimer.Start();

	return true;
}

void CelulaBasica::LoadCellData()
{
	if (cellType == CellType::FIBROBLASTO)
	{
		std::unordered_map<int, std::string> normalAliases = {
			{0, "idle"},
			{12, "walk"},
			{36, "stun"},
			{48, "death"}
		};

		normalAnims.LoadFromTSX("Assets/Textures/Characters/Atlas_fibroplastos_cardiacos.tsx", normalAliases);
		normalAnims.SetCurrent("idle");
		normalAnims.Func_SetAnimationLoop("death", false);

		std::unordered_map<int, std::string> parasiteAliases = {
			{0, "pIdle"},
			{15, "pWalk"},
			{60, "pDeath"},
			{75, "pAttack"}
		};

		parasitizedAnims.LoadFromTSX("Assets/Textures/Characters/Atlas_FibroplastoParasitado.tsx", parasiteAliases);
		parasitizedAnims.SetCurrent("pIdle");
		parasitizedAnims.Func_SetAnimationLoop("pDeath", false);
		parasitizedAnims.Func_SetAnimationLoop("pAttack", false);

		texture = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Atlas_fibroplastos_cardiacos.png");
		parasitizedTexture = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Atlas_FibroplastoParasitado.png");

		texW = 256;
		texH = 256;
	}
}

bool CelulaBasica::Update(float dt)
{
	ZoneScoped;

	GetPhysicsValues();

	if (!canDamagePlayer && damageTimer.ReadMSec() >= damageCooldownMs)
	{
		canDamagePlayer = true;
	}

	Func_CellStates(dt);
	ApplyPhysics();
	Draw(dt);

	return true;
}

void CelulaBasica::GetPhysicsValues()
{
	velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
	velocity = { 0.0f, velocity.y };
}

void CelulaBasica::Func_CellStates(float dt)
{
	switch (currentState)
	{
	case CELL_STATE::IDLE:
		velocity.x = 0.0f;

		if (isParasitized)
		{
			currentState = CELL_STATE::PARASITIZED_CHASING;
			break;
		}

		normalAnims.SetCurrent("idle");

		if (idleTimer.ReadMSec() >= 2000.0f)
		{
			randomDirection.x = (std::rand() % 2 == 0) ? -1.0f : 1.0f;
			randomDirection.y = 0.0f;

			moveTimer.Start();
			currentState = CELL_STATE::MOVING;
		}
		break;

	case CELL_STATE::MOVING:
		if (isParasitized)
		{
			currentState = CELL_STATE::PARASITIZED_CHASING;
			break;
		}

		normalAnims.SetCurrent("walk");
		MoveRandom();

		if (moveTimer.ReadMSec() >= 1200.0f)
		{
			velocity.x = 0.0f;
			idleTimer.Start();
			currentState = CELL_STATE::IDLE;
		}
		break;

	case CELL_STATE::PARASITIZED_CHASING:
		parasitizedAnims.SetCurrent("pWalk");
		MoveParasitized();
		break;

	case CELL_STATE::STUNED:
		velocity.x = 0.0f;

		if (isParasitized)
		{
			parasitizedAnims.SetCurrent("pIdle");
		}
		else
		{
			normalAnims.SetCurrent("stun");
		}

		if (stunTimer.ReadMSec() >= 5000.0f)
		{
			isStunned = false;
			currentState = isParasitized ? CELL_STATE::PARASITIZED_CHASING : CELL_STATE::IDLE;
		}
		break;

	case CELL_STATE::DEATH:
		velocity.x = 0.0f;

		if (isParasitized)
		{
			parasitizedAnims.SetCurrent("pDeath");

			if (parasitizedAnims.Func_HasCurrentAnimationFinished())
			{
				Destroy();
			}
		}
		else
		{
			normalAnims.SetCurrent("death");

			if (normalAnims.Func_HasCurrentAnimationFinished())
			{
				Destroy();
			}
		}
		break;
	}
}

void CelulaBasica::MoveRandom()
{
	velocity.x = randomDirection.x * normalMoveSpeed;

	if (velocity.x > 0.0f)
	{
		isFacingRight = true;
	}
	else if (velocity.x < 0.0f)
	{
		isFacingRight = false;
	}
}

void CelulaBasica::MoveParasitized()
{
	Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
	Vector2D myPos = GetPosition();

	float dx = playerPos.getX() - myPos.getX();

	if (std::abs(dx) > 8.0f)
	{
		velocity.x = dx > 0.0f ? parasitizedMoveSpeed : -parasitizedMoveSpeed;
		isFacingRight = velocity.x > 0.0f;
	}
	else
	{
		velocity.x = 0.0f;
	}
}

void CelulaBasica::ApplyPhysics()
{
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void CelulaBasica::Draw(float dt)
{
	AnimationSet& currentAnims = isParasitized ? parasitizedAnims : normalAnims;
	SDL_Texture* currentTexture = isParasitized ? parasitizedTexture : texture;

	currentAnims.Update(dt);
	const SDL_Rect& animFrame = currentAnims.GetCurrentFrame();

	int x, y;
	pbody->GetPosition(x, y);

	position.setX((float)x);
	position.setY((float)y);

	int frameW = animFrame.w;
	int frameH = animFrame.h;

	int drawX = x - frameW / 2;
	int drawY = y - frameH / 2;

	Engine::GetInstance().render->DrawTexture(
		currentTexture,
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

bool CelulaBasica::IsPlayerDetected() const
{
	if (Engine::GetInstance().scene->player->IsGodMode())
		return false;

	Vector2D playerPosition = Engine::GetInstance().scene->GetPlayerPosition();
	Vector2D cellPosition = const_cast<CelulaBasica*>(this)->GetPosition();

	float dx = playerPosition.getX() - cellPosition.getX();
	float dy = playerPosition.getY() - cellPosition.getY();

	return (dx * dx + dy * dy) <= detectionRange * detectionRange;
}

void CelulaBasica::Parasitize()
{
	if (isParasitized)
		return;

	isParasitized = true;
	currentState = CELL_STATE::PARASITIZED_CHASING;
	parasitizedAnims.SetCurrent("pIdle");
}

void CelulaBasica::SetCellType(CellType type)
{
	cellType = type;
}

void CelulaBasica::SetPosition(Vector2D pos)
{
	if (pbody != nullptr)
	{
		pbody->SetPosition((int)pos.getX(), (int)pos.getY());
	}
}

Vector2D CelulaBasica::GetPosition()
{
	int x, y;
	pbody->GetPosition(x, y);

	return Vector2D((float)x - texW / 2, (float)y - texH / 2);
}

bool CelulaBasica::CleanUp()
{
	if (texture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(texture);
		texture = nullptr;
	}

	if (parasitizedTexture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(parasitizedTexture);
		parasitizedTexture = nullptr;
	}

	if (pbody != nullptr)
	{
		Engine::GetInstance().physics->DeletePhysBody(pbody);
		pbody = nullptr;
	}

	return true;
}

bool CelulaBasica::Destroy()
{
	active = false;
	pendingToDelete = true;
	return true;
}

bool CelulaBasica::IsEnemyStunned()
{
	return currentState == CELL_STATE::STUNED || currentState == CELL_STATE::DEATH;
}

void CelulaBasica::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::SYRINGE:
		if (!isStunned)
		{
			isStunned = true;
			stunTimer.Start();
			currentState = CELL_STATE::STUNED;
		}
		break;

	case ColliderType::PLAYER:
		if (isParasitized && canDamagePlayer)
		{
			Player* playerHit = (Player*)physB->listener;

			if (playerHit != nullptr && !playerHit->IsGodMode())
			{
				playerHit->playerCurrentHp -= contactDamage;

				if (playerHit->playerCurrentHp < 0)
					playerHit->playerCurrentHp = 0;

				canDamagePlayer = false;
				damageTimer.Start();
			}
		}
		break;
	}
}

void CelulaBasica::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
}