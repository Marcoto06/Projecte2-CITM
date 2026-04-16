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

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player() {

}

bool Player::Awake() {
	return true;
}

bool Player::Start() {
	#define PLAYER_BODY_TAG 1
	#define PLAYER_FEET_TAG 2

	// load
	std::unordered_map<int, std::string> aliases = { {0,"idle"},{20,"move"},{40,"puncture"},{49,"extract"},{60,"stop?"},{80, "taptap"}};
	anims.LoadFromTSX("Assets/Textures/Characters/Atlas_Doctora_PNG.tsx", aliases);
	anims.SetCurrent("idle");

	//L03: TODO 2: Initialize Player parameters
	texture = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Atlas_Doctora_PNG.png");

	// L08 TODO 5: Add physics to the player - initialize physics body
	//Engine::GetInstance().textures->GetSize(texture, texW, texH);
	texW = 32;
	texH = 32;
	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect
	pickCoinFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/coin-collision-sound-342335.wav");

	currentState = PLAYERSTATE::IDLE;

	return true;
}

bool Player::Update(float dt)
{
	Draw(dt);
	if (Engine::GetInstance().paused == true) {
		//Engine::GetInstance().physics->SetLinearVelocity(pbody, b2Vec2_zero);
		return true;
	}
	ZoneScoped;
	GetPhysicsValues();
	Move();
	Jump();
	Func_Attacks(dt);
	Teleport();
	ApplyPhysics();

	return true;
}

void Player::Teleport() {
	// Teleport the player to a specific position for testing purposes
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {
		pbody->SetPosition(96, 96);
	}
}

void Player::GetPhysicsValues() {
	// Read current velocity
	velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
	velocity = { 0, velocity.y }; // Reset horizontal velocity by default, this way the player stops when no key is pressed
}

void Player::Move() {

	// Move left/right
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !isSucking) {
		velocity.x = -normalSpeed;
		facingRight = false;
		anims.SetCurrent("move");
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !isSucking) {
		velocity.x = normalSpeed;
		facingRight = true;
		anims.SetCurrent("move");
	}
	
	if (!isJumping && !isSucking && !Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) && !Engine::GetInstance().input->GetKey(SDL_SCANCODE_A))
	{
		anims.SetCurrent("idle");
	}
}

void Player::Jump() {
	// This function can be used for more complex jump logic if needed
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false && !isSucking) {
		Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, 0.0f, -jumpForce, true);
		anims.SetCurrent("jump");
		isJumping = true;
	}
}

void Player::Func_Attacks(float dt) {
	// Stun ATTACK
	if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && !isAttacking) {
		currentState = PLAYERSTATE::ATTACK;
		isAttacking = true;
		attackTimer = 300.0f;
		anims.SetCurrent("attack");
	}

	if (isAttacking) {
		attackTimer -= dt;
		//Destruction
		if (syringeBody != nullptr) {
			Engine::GetInstance().physics->DeletePhysBody(syringeBody);
			syringeBody = nullptr;
		}

		if (attackTimer > 0.0f) {
			float progress = 1.0f - (attackTimer / 300.0f);

			float width = 40.0f;
			float height = 15.0f;

			//Pivot pos
			float pivotLocalX = 15.0f;
			float pivotLocalY = -10.0f;

			float startAngle = -90.0f;
			float endAngle = facingRight ? 0.0f : -180.0f;
			float currentAngle = startAngle + ((endAngle - startAngle) * progress);

			if (!facingRight) {
				pivotLocalX = -pivotLocalX;
			}
			
			//playerpos
			int playerX, playerY;
			pbody->GetPosition(playerX, playerY);

			float distanceToCenter = width / 2.0f;
			float angleRad = currentAngle * (3.14159265f / 180.0f);

			float absoluteCenterX = playerX + pivotLocalX + (distanceToCenter * cos(angleRad));
			float absoluteCenterY = playerY + pivotLocalY + (distanceToCenter * sin(angleRad));

			//create hitbox
			syringeBody = Engine::GetInstance().physics->Func_CreateTemporarySensor(
				(int)width, (int)height, absoluteCenterX, absoluteCenterY, ColliderType::SYRINGE, currentAngle);
		}
		else {
			currentState = PLAYERSTATE::IDLE;
			isAttacking = false;
		}
	}

	// Suck ATTACK
	if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && !isAttacking && !isSucking) {
		currentState = PLAYERSTATE::SUCKING;
		isSucking = true;
		anims.SetCurrent("extract");

		// Crear una hitbox temporal que durará mientras mantengas el click
		int playerX, playerY;
		pbody->GetPosition(playerX, playerY);

		float width = 50.0f;  // Zona de succión ligeramente más grande
		float height = 30.0f;
		float pivotLocalX = facingRight ? 25.0f : -25.0f;

		// IMPORTANTE: Asegúrate de añadir SUCK_ZONE a tu enum ColliderType
		suckBody = Engine::GetInstance().physics->Func_CreateTemporarySensor(
			(int)width, (int)height, playerX + pivotLocalX, playerY, ColliderType::SUCK_ZONE, 0.0f);
	}

	// SUCK ATTACK (MANTENIMIENTO Y FIN)
	if (isSucking) {
		// Comprobar si se suelta el click derecho
		if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_UP) {
			currentState = PLAYERSTATE::IDLE;
			isSucking = false;

			// Destruir la hitbox de succión
			if (suckBody != nullptr) {
				Engine::GetInstance().physics->DeletePhysBody(suckBody);
				suckBody = nullptr;
			}
		}
		else {
			// Si mantenemos el botón, la hitbox debe seguir la posición del jugador
			if (suckBody != nullptr) {
				int playerX, playerY;
				pbody->GetPosition(playerX, playerY);
				float pivotLocalX = facingRight ? 25.0f : -25.0f;
				suckBody->SetPosition((int)(playerX + pivotLocalX), playerY);
			}
		}
	}
}

void Player::ApplyPhysics() {
	// Preserve vertical speed while jumping
	if (isJumping == true) {
		velocity.y = Engine::GetInstance().physics->GetYVelocity(pbody);
	}

	// Apply velocity via helper
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void Player::Draw(float dt) {
	anims.Update(dt);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	// Update render position using your PhysBody helper
	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);

	//L10: TODO 7: Center the camera on the player
	Vector2D mapSize = Engine::GetInstance().map->GetMapSizeInPixels();
	float limitLeft = (float)Engine::GetInstance().render->camera.w / 4;
	float limitRight = (float)mapSize.getX() - Engine::GetInstance().render->camera.w * 3 / 4;
	if (position.getX() - limitLeft > 0 && position.getX() < limitRight) {
		Engine::GetInstance().render->camera.x = (int) - position.getX() + (int)(Engine::GetInstance().render->camera.w / 4);
	}
	else if( position.getX() <= limitLeft) {
		Engine::GetInstance().render->camera.x = 0;
	}
	else {
		Engine::GetInstance().render->camera.x = -(float)mapSize.getX() + Engine::GetInstance().render->camera.w;
	}
	Engine::GetInstance().render->camera.y = (int)-position.getY() + (int)(Engine::GetInstance().render->camera.h / 4 * 3);

	float hitboxW = 32.0f;
	float hitboxH = 32.0f;

	float texW = animFrame.w;
	float texH = animFrame.h;
	
	float drawX = x - (hitboxW / 1.0f);
	float drawY = (y + (hitboxH / 1.0f)) -  texH;

	if (facingRight)
	{
		Engine::GetInstance().render->DrawTexture(texture, drawX, drawY, &animFrame, 1.0f, 0.0, texW / 2, texH / 2, SDL_FLIP_NONE, 1.0f);
		
	}
	else
	{
		Engine::GetInstance().render->DrawTexture(texture, drawX,drawY, &animFrame, 1.0f, 0.0, texW / 2, texH / 2, SDL_FLIP_HORIZONTAL, 1.0f);
	}
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

// L08 TODO 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		//reset the jump flag when touching the ground
		isJumping = false;
		anims.SetCurrent("idle");
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		Engine::GetInstance().audio->PlayFx(pickCoinFxId);
		physB->listener->Destroy();
		hasWallJump = true;
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	case ColliderType::ENEMY:
		//to do
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	case ColliderType::ENEMY:
		//to do
	default:
		break;
	}
}

void Player::ActivateSpeedBoost(float duration, float amount) {
	speedMultiplier = amount;
	boostTimer = duration;
}

Vector2D Player::GetPosition() {
	int x, y;
	pbody->GetPosition(x, y);
	// Adjust for center
	return Vector2D((float)x - texW / 2, (float)y - texH / 2);
}

void Player::SetPosition(Vector2D pos) {
	pbody->SetPosition((int)(pos.getX() + texW / 2), (int)(pos.getY() + texH / 2));
}

bool Player::Destroy()
{
	LOG("Destroying Player");
	active = false;
	pendingToDelete = true;
	return true;
}
