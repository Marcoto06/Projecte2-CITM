#include "Eosinofilo.h"
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
	std::unordered_map<int, std::string> aliases = { {0,"walk"}, {24,"tentaclesIn"}, {48,"idleBoomerang"}, {72,"tentaclesOut"}, {96,"boomerangOut"}, {120,"idleEmpty"}, {144,"boomerangIn"},{168,"stunBoomerang"}, {192,"stunEmpty"}, {216,"hurtBoomerang"}, {240,"hurtEmpty"},{264,"deathBoomerang"},{288,"deathEmpty"} };
	anims.LoadFromTSX("Assets/Textures/Characters/Atlas_Eosinofilo.tsx", aliases);
	anims.SetCurrent("walk");

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

	//Get the position of the enemy
	Vector2D pos = GetPosition();
	//Convert to tile coordinates
	Vector2D tilePos = Engine::GetInstance().map->WorldToMap((int)pos.getX(), (int)pos.getY() + 1);

	return true;
}

bool Eosinofilo::Update(float dt)
{
	ZoneScoped;

	GetPhysicsValues();

	if (!isStunned)
	{
		isPlayerDetected = IsPlayerDetected();

		if (isPlayerDetected)
		{
			currentEState = ENEMYSTATES::ATTACKING;
		}
		else
		{
			currentEState = ENEMYSTATES::PATROLING;
		}
	}

	Vector2D currentPos = GetPosition();
	bool isVisible = Engine::GetInstance().render->IsOnScreenWorldRect(currentPos.getX(), currentPos.getY(), texW, texH, 150);

	Func_EnemyStates(dt);
	ApplyPhysics();
	Draw(dt);

	return true;
}

void Eosinofilo::GetPhysicsValues() {
	// Read current velocity
	velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
	velocity = { 0, velocity.y };
}

void Eosinofilo::Func_EnemyStates(float dt)
{
	switch (currentEState)
	{
	case Eosinofilo::ENEMYSTATES::PATROLING:
		anims.SetCurrent("idleBoomerang");
		Move();
		break;

	case Eosinofilo::ENEMYSTATES::ATTACKING:
		anims.SetCurrent("walk");
		Move();
		break;

	case Eosinofilo::ENEMYSTATES::STUNED:
		anims.SetCurrent("stunBoomerang");

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
				isStunned = false;
			}
		}
		break;

	default:
		break;
	}
}

void Eosinofilo::Move()
{
	velocity.x = 0.0f;

	switch (currentEState)
	{
	case ENEMYSTATES::PATROLING:
		break;

	case ENEMYSTATES::ATTACKING:
	{

		break;
	}

	case ENEMYSTATES::STUNED:
		break;

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