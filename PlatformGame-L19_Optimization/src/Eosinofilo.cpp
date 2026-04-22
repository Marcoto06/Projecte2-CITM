#include "Eosinofilo.h"
#include "Boomerang.h"
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
#include <box2d/box2d.h>
#include <cstdlib>
#include <ctime>
#include <cmath>

Eosinofilo::Eosinofilo() : Entity(EntityType::EOSINOFILO)
{
	name = "Eosinofilo";
}

Eosinofilo::~Eosinofilo() {

}

bool Eosinofilo::Awake() {
	return true;
}

bool Eosinofilo::Start() {

	// load
	//std::unordered_map<int, std::string> aliases = { {0,"walk"}, {30,"idle"}, {60,"hurt"}, {90,"stunned"}, {120,"death"} };
	//anims eosinofilo
	std::unordered_map<int, std::string> aliases = {
	{0,   "move"},
	{24,  "stop"},
	{48,  "idle"},
	{72,  "tentaclesOut"},
	{96,  "boomerangOut"},
	{120, "idleEmpty"},
	{144, "boomerangIn"},
	{168, "stunBoomerang"},
	{192, "stunEmpty"},
	{216, "hurtBoomerang"},
	{240, "hurtEmpty"},
	{264, "deathBoomerang"},
	{288, "deathEmpty"}
	};
	anims.LoadFromTSX("Assets/Textures/Characters/Atlas_Eosinofilo.tsx", aliases);
	anims.SetCurrent("idle");
	anims.Func_SetAnimationLoop("attack", false);
	anims.Func_SetAnimationLoop("stop", false);
	anims.Func_SetAnimationLoop("boomerangOut", false);
	anims.Func_SetAnimationLoop("boomerangIn", false);

	//Initialize Player parameters
	texture = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Atlas_Eosinofilo.png");

	//Add physics to the enemy - initialize physics body
	texW = 64;
	texH = 64;
	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX() + texW / 2, (int)position.getY() + texH / 2, texW / 2, bodyType::DYNAMIC);

	//Assign enemy class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	//ssign collider type
	pbody->ctype = ColliderType::ENEMY;
	b2Body_SetGravityScale(pbody->body, 0.0f);

	//Get the position of the enemy
	Vector2D pos = GetPosition();
	//Convert to tile coordinates
	Vector2D tilePos = Engine::GetInstance().map->WorldToMap((int)pos.getX(), (int)pos.getY() + 1);

	//Reset pathfinding
	static bool randomSeedInitialized = false;
	if (!randomSeedInitialized)
	{
		std::srand((unsigned int)std::time(nullptr));
		randomSeedInitialized = true;
	}

	return true;
}

bool Eosinofilo::Update(float dt)
{
	ZoneScoped;

	GetPhysicsValues();

	if (!isStunned)
	{
		isPlayerDetected = IsPlayerDetected();

		switch (currentEState)
		{
		case ENEMYSTATES::PATROLING:
			if (hasBoomerangEquipped && isPlayerDetected)
			{
				StartAttack();
			}
			break;

		case ENEMYSTATES::ATTACKING:
			if (!hasSpawnedBoomerang && attackTimer.ReadMSec() >= 150.0f)
			{
				hasSpawnedBoomerang = true;
				SpawnBoomerang();
			}

			if (anims.HasCurrentAnimationFinished())
			{
				currentEState = ENEMYSTATES::WAITING_FOR_BOOMERANG;
			}
			break;

		case ENEMYSTATES::WAITING_FOR_BOOMERANG:
			// Stay still here.
			// The boomerang itself will decide whether it returns or is lost.
			break;

		case ENEMYSTATES::RECEIVING:
			if (anims.HasCurrentAnimationFinished())
			{
				hasBoomerangEquipped = true;
				StartRandomMove();
			}
			break;

		case ENEMYSTATES::IDLE_EMPTY:
			if (!boomerangIsActive && !hasBoomerangEquipped && waitTimer.ReadSec() >= 3.0f)
			{
				StartRandomMove();
			}
			break;

		case ENEMYSTATES::MOVING:
			if (moveTimer.ReadSec() >= 3.0f)
			{
				currentEState = ENEMYSTATES::PATROLING;

				if (hasBoomerangEquipped)
				{
					anims.SetCurrent("idle");
				}
				else
				{
					anims.SetCurrent("idleEmpty");
				}
			}
			break;

		case ENEMYSTATES::STUNED:
			break;
		}
	}
	else
	{
		b2Body_SetGravityScale(pbody->body, 100.0f);
		Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, 0, 0);
	}

	Func_EnemyStates(dt);
	ApplyPhysics();
	Draw(dt);

	return true;
}

void Eosinofilo::GetPhysicsValues()
{
	velocity = b2Vec2_zero;
}

void Eosinofilo::Func_EnemyStates(float dt)
{
	switch (currentEState)
	{
	case Eosinofilo::ENEMYSTATES::PATROLING:
		velocity = b2Vec2_zero;
		anims.SetCurrent(hasBoomerangEquipped ? "idle" : "idleEmpty");
		break;

	case Eosinofilo::ENEMYSTATES::ATTACKING:
		velocity = b2Vec2_zero;
		anims.SetCurrent("boomerangOut");
		break;

	case Eosinofilo::ENEMYSTATES::WAITING_FOR_BOOMERANG:
		velocity = b2Vec2_zero;
		anims.SetCurrent(hasBoomerangEquipped ? "idle" : "idleEmpty");
		break;

	case Eosinofilo::ENEMYSTATES::RECEIVING:
		velocity = b2Vec2_zero;
		anims.SetCurrent("boomerangIn");
		break;

	case Eosinofilo::ENEMYSTATES::IDLE_EMPTY:
		velocity = b2Vec2_zero;
		anims.SetCurrent("idleEmpty");
		break;

	case Eosinofilo::ENEMYSTATES::MOVING:
		anims.SetCurrent("move");
		Move();
		break;

	case Eosinofilo::ENEMYSTATES::STUNED:
		anims.SetCurrent(hasBoomerangEquipped ? "stunBoomerang" : "stunEmpty");

		if (isBeingSucked)
		{
			if (suckTimer.ReadMSec() >= 3000.0f)
			{
				Destroy(attackingPlayer);
				return;
			}
		}
		else
		{
			if (timer_01.ReadMSec() > 7000.0f)
			{
				currentEState = ENEMYSTATES::PATROLING;
				b2Body_SetGravityScale(pbody->body, 0.0f);
				isStunned = false;
				anims.SetCurrent(hasBoomerangEquipped ? "idle" : "idleEmpty");
			}
		}
		break;

	default:
		break;
	}
}

void Eosinofilo::Move()
{
	velocity = b2Vec2_zero;

	switch (currentEState)
	{
	case ENEMYSTATES::PATROLING:
		break;

	case ENEMYSTATES::MOVING:
		velocity.x = randomMoveDirection.x * moveSpeed;
		velocity.y = randomMoveDirection.y * moveSpeed;

		if (velocity.x > 0.0f)
		{
			isFacingRight = true;
		}
		else if (velocity.x < 0.0f)
		{
			isFacingRight = false;
		}
		break;

	case ENEMYSTATES::ATTACKING:
	case ENEMYSTATES::IDLE_EMPTY:
	case ENEMYSTATES::RECEIVING:
	case ENEMYSTATES::STUNED:
	default:
		break;
	}
}

bool Eosinofilo::IsPlayerDetected() const
{
	Vector2D playerPosition = Engine::GetInstance().scene->GetPlayerPosition();
	Vector2D enemyPosition = const_cast<Eosinofilo*>(this)->GetPosition();

	float distanceX = playerPosition.getX() - enemyPosition.getX();
	float distanceY = playerPosition.getY() - enemyPosition.getY();
	float squaredDistance = (distanceX * distanceX) + (distanceY * distanceY);

	return squaredDistance <= (detectionRange * detectionRange);
}

void Eosinofilo::StartAttack()
{
	currentEState = ENEMYSTATES::ATTACKING;
	attackTimer.Start();
	hasSpawnedBoomerang = false;
	velocity = b2Vec2_zero;
	anims.SetCurrent("boomerangOut");
}

// Implementation of the SpawnBoomerang function that creates a new Boomerang entity, sets its position and direction based on the Eosinofilo's current state, and adds it to the EntityManager.
void Eosinofilo::SpawnBoomerang()
{
	std::shared_ptr<Boomerang> boomerang = std::make_shared<Boomerang>();

	boomerang->Start();

	Vector2D eosPosition = GetPosition();
	Vector2D playerPosition = Engine::GetInstance().scene->GetPlayerPosition();

	float spawnOffsetY = 20.0f;

	Vector2D spawnPosition(
		eosPosition.getX(),
		eosPosition.getY() + spawnOffsetY
	);

	Vector2D directionToPlayer(
		playerPosition.getX() - spawnPosition.getX(),
		playerPosition.getY() - spawnPosition.getY()
	);

	if (directionToPlayer.getX() >= 0.0f)
	{
		isFacingRight = true;
	}
	else
	{
		isFacingRight = false;
	}

	boomerang->SetOwner(this);
	boomerang->SetSpawnPosition(spawnPosition);
	boomerang->SetDirection(directionToPlayer);

	boomerangIsActive = true;
	hasBoomerangEquipped = false;

	Engine::GetInstance().entityManager->AddEntity(boomerang);
}

void Eosinofilo::StartRandomMove()
{
	SetRandomMoveDirection();
	moveTimer.Start();
	currentEState = ENEMYSTATES::MOVING;
	anims.SetCurrent("move");
}

void Eosinofilo::StartIdleEmpty()
{
	currentEState = ENEMYSTATES::IDLE_EMPTY;
	hasBoomerangEquipped = false;
	anims.SetCurrent("idleEmpty");
	waitTimer.Start();
}

void Eosinofilo::RestoreBoomerang()
{
	hasBoomerangEquipped = true;
	boomerangIsActive = false;
	currentEState = ENEMYSTATES::PATROLING;
	anims.SetCurrent("idle");
}

void Eosinofilo::OnBoomerangReturned()
{
	boomerangIsActive = false;
	hasBoomerangEquipped = false;
	currentEState = ENEMYSTATES::RECEIVING;
	anims.SetCurrent("boomerangIn");
}

void Eosinofilo::OnBoomerangLost()
{
	boomerangIsActive = false;
	hasBoomerangEquipped = false;
	currentEState = ENEMYSTATES::IDLE_EMPTY;
	anims.SetCurrent("idleEmpty");
	waitTimer.Start();
}

void Eosinofilo::SetRandomMoveDirection()
{
	int dirX = 0;
	int dirY = 0;

	while (dirX == 0 && dirY == 0)
	{
		dirX = (std::rand() % 3) - 1;
		dirY = (std::rand() % 3) - 1;
	}

	float length = std::sqrt((float)(dirX * dirX + dirY * dirY));

	randomMoveDirection.x = dirX / length;
	randomMoveDirection.y = dirY / length;
}

void Eosinofilo::ApplyPhysics() {

	// Apply velocity via helper
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void Eosinofilo::Draw(float dt)
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
	int drawY = y - (frameH / 2) - 82;

	if (isFacingRight)
	{
		Engine::GetInstance().render->DrawTexture(
			texture,
			drawX,
			drawY,
			&animFrame,
			1.0f,
			0.0,
			frameW / 2,
			frameH / 2,
			SDL_FLIP_HORIZONTAL,
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
			frameW / 2,
			frameH / 2,
			SDL_FLIP_NONE,
			1.0f
		);
	}
}

bool Eosinofilo::CleanUp()
{
	LOG("Cleanup eosinofilo");
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

bool Eosinofilo::Destroy()
{
	LOG("Destroying Eosinofilo");
	active = false;
	pendingToDelete = true;
	return true;
}

bool Eosinofilo::Destroy(Player* player) // Good: coincide with the .h
{
	LOG("Destroying Enemy");

	if (player != nullptr) {
		player->ActivateSpeedBoost();
	}

	return Destroy();
}

void Eosinofilo::SetPosition(Vector2D pos) {
	pbody->SetPosition((int)(pos.getX()), (int)(pos.getY()));
}

Vector2D Eosinofilo::GetPosition() {
	int x, y;
	pbody->GetPosition(x, y);
	// Adjust for center
	return Vector2D((float)x - texW / 2, (float)y - texH / 2);
}

//Define OnCollision function for the enemy. 
void Eosinofilo::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::SYRINGE:
		if (!isStunned)
		{
			timer_01.Start();
			currentEState = ENEMYSTATES::STUNED;
			isStunned = true;
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

void Eosinofilo::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::SUCK_ZONE:
		// Stop sucking if player releases right click
		isBeingSucked = false;
		break;
	}
}