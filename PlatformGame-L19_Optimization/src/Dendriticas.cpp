#include "Dendriticas.h"
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

Dendriticas::Dendriticas() : Entity(EntityType::DENDRITICAS)
{
	name = "Dendriticas";
}

Dendriticas::~Dendriticas() {

}

bool Dendriticas::Awake() {
	return true;
}

bool Dendriticas::Start() {

	// load
	std::unordered_map<int, std::string> aliases = { {0,"idle"}, {25,"walk"}, {50,"hurt"}, {75,"stunned"}, {100,"death"}, {125,"attack"} , {150, "alarmOn"} };
	anims.LoadFromTSX("Assets/Textures/Characters/Atlas_Dendriticas.tsx", aliases);
	anims.SetCurrent("walk");
	anims.Func_SetAnimationLoop("death", false);
	anims.Func_SetAnimationLoop("attack", false);

	//Initialize Player parameters
	texture = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Atlas_Dendriticas.png");

	//Add physics to the enemy - initialize physics body
	texW = 125;
	texH = 200;
	pbody = Engine::GetInstance().physics->CreateRectangle((int)position.getX() + texW / 2, ((int)position.getY() + texH / 2), texW, texH, bodyType::DYNAMIC);
	pbody->SetFixedRotation(true);

	//Assign enemy class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	//ssign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Initialize pathfinding
	pathfinding = std::make_shared<Pathfinding>();
	//Get the position of the enemy
	Vector2D pos = GetPosition();
	//Convert to tile coordinates
	Vector2D tilePos = Engine::GetInstance().map->WorldToMap((int)pos.getX(), (int)pos.getY() + 1);
	//Reset pathfinding
	pathfinding->ResetPath(tilePos);
	player = Engine::GetInstance().scene->player.get();

	return true;
}

bool Dendriticas::Update(float dt)
{
	ZoneScoped;

	GetPhysicsValues();

	if (!isStunned && currentEState != ENEMYSTATES::ATTACK)
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

void Dendriticas::PerformPathfinding()
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

void Dendriticas::GetPhysicsValues() {
	// Read current velocity
	velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
	velocity = { 0, velocity.y };
}

void Dendriticas::Func_EnemyStates(float dt)
{
	switch (currentEState)
	{
	case Dendriticas::ENEMYSTATES::WALKING:
		anims.SetCurrent("idle");
		Move();
		break;

	case Dendriticas::ENEMYSTATES::CHASING:
		anims.SetCurrent("walk");
		Move();
		break;

	case Dendriticas::ENEMYSTATES::STUNED:
		anims.SetCurrent("stunned");

		if (isBeingSucked)
		{
			if (!player->isAdrenaline)
			{
				if (suckTimer.ReadMSec() >= 3000.0f)
				{
					currentEState = ENEMYSTATES::DEATH;
					return;


				}
			}
			else
			{
				if (suckTimer.ReadMSec() >= 1500.0f)
				{
					currentEState = ENEMYSTATES::DEATH;
					return;


				}
			}
		}
		else
		{
			if (timer_01.ReadMSec() > 7000.0f)
			{
				currentEState = ENEMYSTATES::WALKING;
				isStunned = false;
			}
		}
		break;

	case Dendriticas::ENEMYSTATES::DEATH:

		anims.SetCurrent("death");
		if (anims.Func_HasCurrentAnimationFinished())
		{
			Destroy(attackingPlayer);
			return;
		}
		break;
	case Dendriticas::ENEMYSTATES::ATTACK:
		anims.SetCurrent("attack");
		
		if (anims.Func_HasCurrentAnimationFinished())
		{
			currentEState = ENEMYSTATES::WALKING;
		}
		break;
	default:
		break;
	}
}

void Dendriticas::Move()
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

bool Dendriticas::IsPlayerDetected() const
{
	if (Engine::GetInstance().scene->player->IsGodMode())
		return false;

	Vector2D playerPosition = Engine::GetInstance().scene->GetPlayerPosition();
	Vector2D enemyPosition = const_cast<Dendriticas*>(this)->GetPosition();

	float distanceX = playerPosition.getX() - enemyPosition.getX();
	float distanceY = playerPosition.getY() - enemyPosition.getY();
	float squaredDistance = (distanceX * distanceX) + (distanceY * distanceY);

	return squaredDistance <= (detectionRange * detectionRange);
}

Vector2D Dendriticas::GetNextPathTile() const
{
	const std::list<Vector2D>& pathTiles = pathfinding->GetPathTiles();

	if (pathTiles.size() <= 1)
	{
		return Vector2D(-1, -1);
	}

	auto nextTileIt = pathTiles.rbegin();
	++nextTileIt; // first reverse element is the current/origin tile, second is the next tile to follow

	return *nextTileIt;
}

void Dendriticas::ApplyPhysics() {

	// Apply velocity via helper
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void Dendriticas::Draw(float dt)
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
	int drawY = y - (frameH / 2) - 80;

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
			(frameW / 2),
			(frameH / 2),
			SDL_FLIP_NONE,
			1.0f
		);
	}
}

bool Dendriticas::CleanUp()
{
	LOG("Cleanup enemy");
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

bool Dendriticas::Destroy()
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

bool Dendriticas::Destroy(Player* pplayer) // Good: coincide with the .h
{
	player->isAdrenaline;
	player->effectAnims.SetCurrent("lifeUp");
	return Destroy();
}

void Dendriticas::SetPosition(Vector2D pos) {
	pbody->SetPosition((int)(pos.getX()), (int)(pos.getY()));
}

Vector2D Dendriticas::GetPosition() {
	int x, y;
	pbody->GetPosition(x, y);
	// Adjust for center
	return Vector2D((float)x - texW / 2, (float)y - texH / 2);
}

bool Dendriticas::IsEnemyStunned() {

	if (currentEState == ENEMYSTATES::STUNED or currentEState == ENEMYSTATES::DEATH)
	{
		return true;
	}
	else return false;
}


//Define OnCollision function for the enemy. 
void Dendriticas::OnCollision(PhysBody* physA, PhysBody* physB) {
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
	case ColliderType::PLAYER:
		if (currentEState != ENEMYSTATES::DEATH && currentEState != ENEMYSTATES::STUNED)
		{
			currentEState = ENEMYSTATES::ATTACK;
			attackingPlayer = (Player*)physB->listener;
		}
		break;
	}
}

void Dendriticas::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::SUCK_ZONE:
		// Stop sucking if player releases right click
		isBeingSucked = false;
		break;
	}
}