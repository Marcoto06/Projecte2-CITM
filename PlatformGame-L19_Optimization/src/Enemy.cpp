#include "Enemy.h"
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

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name = "Enemy";
}

Enemy::~Enemy() {

}

bool Enemy::Awake() {
	return true;
}

bool Enemy::Start() {

	// load
	std::unordered_map<int, std::string> aliases = { {0,"idle"}, {1,"stuned"}};
	anims.LoadFromTSX("Assets/Textures/enemy_Spritesheet.tsx", aliases);
	anims.SetCurrent("idle");

	//Initialize Player parameters
	texture = Engine::GetInstance().textures->Load("Assets/Textures/enemy_spritesheet.png");

	//Add physics to the enemy - initialize physics body
	texW = 32;
	texH = 32;
	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX()+texW/2, (int)position.getY()+texH/2, texW / 2, bodyType::DYNAMIC);

	//Assign enemy class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	//ssign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Initialize pathfinding
	pathfinding = std::make_shared<Pathfinding>();
	//Get the position of the enemy
	Vector2D pos = GetPosition();
	//Convert to tile coordinates
	Vector2D tilePos = Engine::GetInstance().map->WorldToMap((int)pos.getX(), (int)pos.getY()+1);
	//Reset pathfinding
	pathfinding->ResetPath(tilePos);

	return true;
}

bool Enemy::Update(float dt)
{
	ZoneScoped;
	PerformPathfinding();
	GetPhysicsValues();
	Func_EnemyStates(dt);
	ApplyPhysics();
	Draw(dt);

	return true;
}

void Enemy::PerformPathfinding() {

	//Get the position of the enemy
	Vector2D pos = GetPosition();
	//Convert to tile coordinates
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap((int)pos.getX(), (int)pos.getY());
	//Reset pathfinding
	pathfinding->ResetPath(tilePos);

	while(pathfinding->CanPropagateAStar(tilePos)) {
		pathfinding->PropagateAStar(SQUARED);
	}
}

void Enemy::GetPhysicsValues() {
	// Read current velocity
	velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
	velocity = { 0, velocity.y }; 
}

void Enemy::Func_EnemyStates(float dt)
{
	switch (currentEState)
	{
	case Enemy::ENEMYSTATES::WALKING:
		Move();
		break;
	case Enemy::ENEMYSTATES::CHASING:
		break;
	case Enemy::ENEMYSTATES::STUNED:
		LOG("STUNNED");
		//put a timer to change back to walking
		anims.SetCurrent("stuned");
		break;
	default:
		break;
	}
}

void Enemy::Move() {

	velocity.x = -speed;
	// Move 
}

void Enemy::ApplyPhysics() {

	// Apply velocity via helper
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void Enemy::Draw(float dt) {

	anims.Update(dt);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	// Update render position using your PhysBody helper
	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);

	// Draw pathfinding debug
	//pathfinding->DrawPath();

	//Draw the player using the texture and the current animation frame

	if (isFacingRight) //this depends on how is the sprite made
	{
		Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2, &animFrame);
	}
	else
	{
		Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2, &animFrame, 1.0f, 0.0, texW / 2, texH / 2, SDL_FLIP_HORIZONTAL);
	}
}

bool Enemy::CleanUp()
{
	LOG("Cleanup enemy");
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

bool Enemy::Destroy()
{
	LOG("Destroying Enemy");
	active = false;
	pendingToDelete = true;
	return true;
}

void Enemy::SetPosition(Vector2D pos) {
	pbody->SetPosition((int)(pos.getX()), (int)(pos.getY()));
}

Vector2D Enemy::GetPosition() {
	int x, y;
	pbody->GetPosition(x, y);
	// Adjust for center
	return Vector2D((float)x-texW/2,(float)y-texH/2);
}

//Define OnCollision function for the enemy. 
void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::SYRINGE:
		if (!isStuned)
		{
			currentEState = ENEMYSTATES::STUNED;
			isStuned = true;
		}
		else
		{
			//add func_suckattack thing 
		}
		break;
	}
}

void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	//idk if i should keep this here or in OnCollision regular
	//switch (physB->ctype)
	//{
	//case ColliderType::SYRINGE:
	//	if (!isStuned)
	//	{
	//		currentEState = ENEMYSTATES::STUNED;
	//		isStuned = true;
	//	}
	//	else
	//	{
	//		//add func_suckattack thing 
	//	}
	//	break;
	//}
}
