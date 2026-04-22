#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Enemy.h"
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
	playerCurrentHp = playerMaxHp;
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
	std::unordered_map<int, std::string> aliases = { {0,"idle"},{21,"run"},{42,"absorb"},{51,"extract"},{63,"endabsorb"},{84, "taptap"},{105, "prepareJump"},{111, "jumping"},{115, "jumping2"},{118, "fallingJump"},{122, "endJump"},{126, "climb"}, {147, "stun"}, {168, "airAttack"},{189, "hurt"}, {210, "crouch"}, {231, "death"}, {252, "spark"} };
	anims.LoadFromTSX("Assets/Textures/Characters/Atlas_Doctora.tsx", aliases);
	anims.SetCurrent("idle"); 

	anims.Func_SetAnimationLoop("absorb", false);
	
	anims.Func_SetAnimationLoop("endabsorb", false);
	anims.Func_SetAnimationLoop("taptap", false);
	anims.Func_SetAnimationLoop("prepareJump", false);
	
	anims.Func_SetAnimationLoop("jumping2", false);
	
	anims.Func_SetAnimationLoop("endJump", false);

	anims.Func_SetAnimationLoop("stun", false);
	anims.Func_SetAnimationLoop("airAttack", false);

	anims.Func_SetAnimationLoop("death", false);
	texture = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Atlas_Doctora.png");

	texW = 96;
	texH = 168;
	pbody = Engine::GetInstance().physics->CreateRectangle((int)position.getX(), (int)position.getY(), texW / 2, texH - 40, bodyType::DYNAMIC);

	pbody->SetFixedRotation(true);
	pbody->listener = this;

	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect
	pickCoinFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/coin-collision-sound-342335.wav");

	currentState = PLAYERSTATE::IDLE;

	return true;
}

bool Player::Update(float dt)
{
	/*LOG("%f", velocity.x);*/
	Draw(dt);
	if (Engine::GetInstance().paused == true) {
		//Engine::GetInstance().physics->SetLinearVelocity(pbody, b2Vec2_zero);
		return true;
	}

	UpdateAttack(dt);
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		Attack();
	}

	ZoneScoped;

	if (isHurt) {
		if (hurtTimer.ReadMSec() > 300.0f) {
			isHurt = false;
		}
	}
	GetPhysicsValues();

	if (!isHurt) {
		if (hasASpeedBoost) {
			Func_BoostMovement();
		}
		else {
			Move();
		}
		Jump();
	}

	Jump();
	Func_PlayerState();
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

	if (!isHurt) {
		velocity = { 0, velocity.y };
	}

}

void Player::Attack()
{
	if (isAttacking) return;
	if (attackCooldownTimer > 0.0f) return;

	isAttacking = true;
	attackTimer = attackDuration;
	attackCooldownTimer = attackCooldown;
	attackHitBoxActive = false;

	currentState = PLAYERSTATE::ATTACK;

	// Later:
	// anims.SetCurrent("attack");
}

void Player::UpdateAttack(float dt)
{
	if (attackCooldownTimer > 0.0f)
	{
		attackCooldownTimer -= dt;
		if (attackCooldownTimer < 0.0f)
			attackCooldownTimer = 0.0f;
	}

	if (!isAttacking) return;

	attackTimer -= dt;

	float elapsed = attackDuration - attackTimer;

	if (elapsed >= 0.05f && elapsed <= 0.12f)
	{
		if (!attackHitBoxActive)
		{
			attackHitBoxActive = true;
			StartAttackHitBox();
		}
	}
	else
	{
		if (attackHitBoxActive)
		{
			attackHitBoxActive = false;
			StopAttackHitBox();
		}
	}

	if (attackTimer <= 0.0f)
	{
		attackTimer = 0.0f;
		isAttacking = false;

		if (attackHitBoxActive)
		{
			attackHitBoxActive = false;
			StopAttackHitBox();
		}

		if (!isJumping)
			currentState = PLAYERSTATE::IDLE;
	}
}

void Player::StartAttackHitBox()
{
	// Create the attack hitbox
}

void Player::StopAttackHitBox()
{
	// Destroy the attack hitbox
}

void Player::Move() {

	isMoving = false; // Reseteamos cada frame

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !isSucking && canMove) {
		isMoving = true;
		velocity.x = -normalSpeed;
		facingRight = false;
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !isSucking && canMove) {
		isMoving = true;
		velocity.x = normalSpeed;
		facingRight = true;
	}
	if (nextToWall == true && velocity.y != 0.0f) {
		velocity.x = 0;
	}
}

void Player::ActivateSpeedBoost() {
	hasASpeedBoost = true;
	boostTimer_01.Start();
	LOG("Boost iniciado!");
}

void Player::Func_BoostMovement() {
	float durationMS = 5000.0f; // 5 seconds in miliseconds
	isMoving = false; 

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !isSucking && canMove) {
		isMoving = true;
		velocity.x = -boostSpeed;
		facingRight = false;
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !isSucking && canMove) {
		isMoving = true;
		velocity.x = boostSpeed;
		facingRight = true;
	}

	if (boostTimer_01.ReadMSec() > durationMS)
	{
		hasASpeedBoost = false;
		LOG("Boost terminado");
	}
}

void Player::Jump() {
	// This function can be used for more complex jump logic if needed
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false && !isSucking && onGround == true && canJump) {
		currentState = PLAYERSTATE::PREPARE_JUMP;
		float forceToUse = jumpForce;
		if (hasPowerJump == true) {
			forceToUse = powerJumpForce;
		}
		Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, 0.0f, -forceToUse, true);
		anims.SetCurrent("prepareJump");
		isJumping = true;
		onGround = false;
		nextToWall = true;
	}
}

void Player::Func_PlayerState() {
	
	if (currentState == PLAYERSTATE::DEATH) {
		if (anims.HasCurrentAnimationFinished()) 
		{
			//Destroy();
			Engine::GetInstance().scene->ActivateGameOver();
		}
		return;
	}

	if (currentState == PLAYERSTATE::FALLING_JUMP && onGround && !isAttacking)
	{
		currentState = PLAYERSTATE::END_JUMP;
 		anims.SetCurrent("endJump");
	}
	else if (currentState == PLAYERSTATE::JUMPING && velocity.y > 0.1f && !isAttacking)
	{
		currentState = PLAYERSTATE::FALLING_JUMP;
		anims.SetCurrent("fallingJump");
	}
	else if ((currentState == PLAYERSTATE::IDLE || currentState == PLAYERSTATE::MOVE) && !onGround && !isAttacking)
	{
		currentState = PLAYERSTATE::FALLING_JUMP;
		anims.SetCurrent("fallingJump");
	}

	switch (currentState)
	{
	case Player::PLAYERSTATE::PREPARE_JUMP:
		if (anims.HasCurrentAnimationFinished())
		{
			currentState = PLAYERSTATE::JUMPING;
			anims.SetCurrent("jumping");
		}
		break;
	case Player::PLAYERSTATE::END_JUMP:
		if (anims.HasCurrentAnimationFinished())
		{
			isJumping = false;
			if (isMoving) {
				currentState = PLAYERSTATE::MOVE;
				anims.SetCurrent("run"); 
			}
			else {
				currentState = PLAYERSTATE::IDLE;
				anims.SetCurrent("idle");
			}
		}
		break;
	case Player::PLAYERSTATE::IDLE:
		if (onGround && isMoving) {
			currentState = Player::PLAYERSTATE::MOVE;
			anims.SetCurrent("run"); 
		}
		else
		{
			anims.SetCurrent("idle");
		}
		break;

	case Player::PLAYERSTATE::MOVE:
		if (onGround && !isMoving) {
			currentState = Player::PLAYERSTATE::IDLE;
			anims.SetCurrent("idle");
		}
		break;

	default:
		break;
	}
}

void Player::Func_Attacks(float dt) {
	// Stun ATTACK
	// Left click attack
	if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && !isAttacking && !isSucking && canAttack)
	{
		currentState = PLAYERSTATE::ATTACK;
		isAttacking = true;

		if (!onGround)
		{
			attackTimer = 1000.0f;
			anims.SetCurrent("airAttack");
		}
		else
		{
			attackTimer = 1300.0f;
			anims.SetCurrent("stun");
		}
	}

	if (isAttacking) {
		attackTimer -= dt;

		if (syringeBody != nullptr) {
			Engine::GetInstance().physics->DeletePhysBody(syringeBody);
			syringeBody = nullptr;
		}

		if (attackTimer <= 200.0f && attackTimer > 0.0f) {

			float progress = 1.0f - (attackTimer / 200.0f);

			float width = 120.0f;
			float height = 30.0f;

			float pivotLocalX = 15.0f;
			float pivotLocalY = -10.0f;

			float startAngle = 90.0f;
			float endAngle = facingRight ? -90.0f : 270.0f;

			float currentAngle = startAngle + ((endAngle - startAngle) * progress);

			if (!facingRight) {
				pivotLocalX = -pivotLocalX;
			}

			int playerX, playerY;
			pbody->GetPosition(playerX, playerY);

			float distanceToCenter = width / 2.0f;
			float angleRad = currentAngle * (3.14159265f / 180.0f);

			float absoluteCenterX = playerX + pivotLocalX + (distanceToCenter * cos(angleRad));
			float absoluteCenterY = playerY + pivotLocalY + (distanceToCenter * sin(angleRad));

			syringeBody = Engine::GetInstance().physics->Func_CreateTemporarySensor(
				(int)width, (int)height, absoluteCenterX, absoluteCenterY, ColliderType::SYRINGE, angleRad);
		}
		else if (attackTimer <= 0.0f)
		{
			attackTimer = 0.0f;
			isAttacking = false;

			if (!onGround)
			{
				currentState = PLAYERSTATE::FALLING_JUMP;
				anims.SetCurrent("fallingJump");
			}
			else if (isMoving)
			{
				currentState = PLAYERSTATE::MOVE;
				anims.SetCurrent("run");
			}
			else
			{
				currentState = PLAYERSTATE::IDLE;
				anims.SetCurrent("idle");
			}
		}
	}
	else {
		if (syringeBody != nullptr) {
			Engine::GetInstance().physics->DeletePhysBody(syringeBody);
			syringeBody = nullptr;
		}
	}

	if (anims.HasCurrentAnimationFinished())
	{
		currentState = PLAYERSTATE::IDLE;
	}

	/*if (anims.HasCurrentAnimationFinished())
	{
		if (isAttacking)
		{
			if (!onGround)
			{
				currentState = PLAYERSTATE::FALLING_JUMP;
				anims.SetCurrent("fallingJump");
			}
			else if (isMoving)
			{
				currentState = PLAYERSTATE::MOVE;
				anims.SetCurrent("run");
			}
			else
			{
				currentState = PLAYERSTATE::IDLE;
				anims.SetCurrent("idle");
			}

			isAttacking = false;
		}
	}*/

	// Suck ATTACK
	if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && !isAttacking && !isSucking && canAttack) {
		currentState = PLAYERSTATE::SUCKING;
		isSucking = true;
		anims.SetCurrent("extract");

		int playerX, playerY;
		pbody->GetPosition(playerX, playerY);

		float width = 55.0f;  
		float height = 90.0f;
		float pivotLocalX = facingRight ? 52.5f : -52.5f;

		suckBody = Engine::GetInstance().physics->Func_CreateTemporarySensor(
			(int)width, (int)height, pivotLocalX, playerY, ColliderType::SUCK_ZONE, 0.0f);
		suckBody->listener = this;
	}

	if (isSucking) {
		if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_UP) {
			currentState = PLAYERSTATE::IDLE;
			isSucking = false;

			if (suckBody != nullptr) {
				Engine::GetInstance().physics->DeletePhysBody(suckBody);
				suckBody = nullptr;
			}
		}
		else {
			if (suckBody != nullptr) {
				int playerX, playerY;
				pbody->GetPosition(playerX, playerY);
				float pivotLocalX = facingRight ? 52.5f : -52.5f;
				suckBody->SetPosition((int)(playerX + pivotLocalX), playerY + 22);
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
	float limitUp = (float)Engine::GetInstance().render->camera.h - 320;
	float limitDown = (float)mapSize.getY() - 320;
	//LIMIT X CAMERA
	if (position.getX() - limitLeft > 0 && position.getX() < limitRight) {
		Engine::GetInstance().render->camera.x = (int) - position.getX() + (int)(Engine::GetInstance().render->camera.w / 4);
	}
	else if( position.getX() <= limitLeft) {
		Engine::GetInstance().render->camera.x = 0;
	}
	else {
		Engine::GetInstance().render->camera.x = -(float)mapSize.getX() + Engine::GetInstance().render->camera.w;
	}
	//LIMIT Y CAMERA
	if (position.getY() - limitUp > 0 && position.getY() < limitDown) {
		Engine::GetInstance().render->camera.y = (int)-position.getY() + (int)(Engine::GetInstance().render->camera.h - 320);
	}
	else if (position.getY() <= limitUp) {
		Engine::GetInstance().render->camera.y = 0;
	}
	else {
		Engine::GetInstance().render->camera.y = -(float)mapSize.getY() + Engine::GetInstance().render->camera.h;
	}
	//Engine::GetInstance().render->camera.y = (int)-position.getY() + (int)(Engine::GetInstance().render->camera.h / 4 * 3);

	float texW = animFrame.w;
	float texH = animFrame.h;
	
	if (facingRight)
	{
		Engine::GetInstance().render->DrawTexture(texture, position.getX() - 258, position.getY() - 432, &animFrame, 1.0f, 0.0, texW / 2, texH / 2, SDL_FLIP_NONE, 1.0f);
		
	}
	else
	{
		Engine::GetInstance().render->DrawTexture(texture, position.getX() - 258, position.getY() - 432, &animFrame, 1.0f, 0.0, texW / 2, texH / 2, SDL_FLIP_HORIZONTAL, 1.0f);
	}
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}
 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	Enemy* enemy;
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		if (velocity.y >= -0.1f) {
			isJumping = false;
			onGround = true;
		}
		else if (velocity.y != 0.0f){
			nextToWall = true;
		}
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
		LOG("End Collision ENEMY");

		if (!isHurt && currentState != PLAYERSTATE::DEATH)
		{
			playerCurrentHp--;
			LOG("Current HP: %i", playerCurrentHp);

			if (playerCurrentHp > 0)
			{
				int playerX, playerY;
				pbody->GetPosition(playerX, playerY);

				int enemyX, enemyY;
				physB->GetPosition(enemyX, enemyY);

				float knockbackX = (playerX < enemyX) ? -7.0f : 7.0f;
				float knockbackY = -5.0f;

				Engine::GetInstance().physics->SetLinearVelocity(pbody, { knockbackX, knockbackY });

				isHurt = true;
				hurtTimer.Start();
			}
			else
			{
				LOG("Player has died!");
				canMove = false;
				canJump = false;
				canAttack = false;
				currentState = PLAYERSTATE::DEATH;
				anims.SetCurrent("death");
			}
		}		
		break;
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
		onGround = false;
		nextToWall = false;
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	default:
		break;
	}
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
