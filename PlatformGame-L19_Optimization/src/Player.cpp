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
#include "Window.h"
#include "tracy/Tracy.hpp"

/* ----------------------- CONTROLS (Keyboard/Gamepad) ----------------------- 
	-- Player --
	Movement: A/D | Left Analog
	Jump: Space | B
	Stun attack: Left Click | X
	Suck attack: Right Click | Y
	Dash: E
	Crouch: Left Shift
	
	-- Menus -- 
	Move Up/Down: W/S | Dpad Up/Down
	Slider Left/Right: W/S | Dpad Left/Right
	Accept: E | A
*/

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
	playerCurrentHp = playerMaxHp;
}

Player::~Player() {

}

bool Player::Awake() {
	Vector2D defaultPosition = GetPosition();
	SetRespawnPosition(defaultPosition);

	return true;
}

bool Player::Start() {
	#define PLAYER_BODY_TAG 1
	#define PLAYER_FEET_TAG 2

	// load
	std::unordered_map<int, std::string> aliases = { {0,"idle"},{21,"run"},{42,"absorb"},{51,"extract"},{63,"endabsorb"},{84, "taptap"},{105, "prepareJump"},{111, "jumping"},{115, "jumping2"},{118, "fallingJump"},{122, "endJump"},{126, "climb"}, {147, "stun"}, {168, "airAttack"},{189, "hurt"}, {210, "crouch"}, {231, "death"}, {252, "spark"}, {273, "jumpCascada"},{279, "insideCascada"},{289, "outCascada"} };
	anims.LoadFromTSX("Assets/Textures/Characters/Atlas_Doctora.tsx", aliases);
	std::unordered_map<int, std::string> effects = { {0,"lifeUp"}, {16, "aux"} };
	effectAnims.LoadFromTSX("Assets/Textures/UI/InGameUI/Atlas_LifeUp.tsx", effects);
	effectAnims.SetCurrent("lifeUp");
	effectAnims.Func_SetAnimationLoop("lifeUp", false);

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

	anims.Func_SetAnimationLoop("jumpCascada", false);
	anims.Func_SetAnimationLoop("insideCascada", true);
	anims.Func_SetAnimationLoop("outCascada", false);

	texture = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Atlas_Doctora.png");
	healText = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Atlas_LifeUp.png");

	texW = 96;
	texH = 168;
	//Hitbox
	pbody = Engine::GetInstance().physics->CreateRectangle((int)position.getX(), (int)position.getY() + 25, texW / 2, texH -50, bodyType::DYNAMIC);

	pbody->SetFixedRotation(true);
	pbody->listener = this;

	pbody->ctype = ColliderType::PLAYER;
	gravityScale = b2Body_GetGravityScale(pbody->body);

	floorSensorBody = Engine::GetInstance().physics->Func_CreateTemporarySensor(texW / 3, 10, (int)position.getX() + texW / 6, (int)position.getY() + 175, ColliderType::SENSOR);
	floorSensorBody->listener = this;

	walkFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx doc/walk_doc.wav");
	hurtFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx doc/hurt_doc.wav");
	deathFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx doc/death_doc.wav");
	caerFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx doc/Doctora_Caer.wav");
	UIdeathID = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx UI/mort.wav");
	stunAttackFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx doc/stun_attack.wav");
	jumpFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx doc/Doctora_Salto.wav");
	stunAttackDocFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx doc/stun_attack_doc.wav");
	suckAttackFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx doc/Absorver.wav");
	dashFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx doc/dash_doc.wav");

	currentState = PLAYERSTATE::IDLE;
	onGround = true;

	return true;
}

bool Player::Update(float dt)
{
	Draw(dt);
	if (playerCurrentHp <= 0) currentState = PLAYERSTATE::DEATH;
	if (lock) return true;

	if (isCamouflage) {
		if (camouflageTimer.ReadMSec() > 20000.0f) {
			isCamouflage = false;
		}
	}

	if (hasDash && dashing == false && dashLeft > 0 && !isClimbing) {
		Func_Dash();
	} 
	if (dashing == true) {
		if (dashTimer.ReadMSec() > 250) {
			dashing = false;
			b2Body_SetGravityScale(pbody->body, gravityScale);
			velocity.x = 0;
			velocity.y = 0;
		}
	}
	if (Engine::GetInstance().paused == true || dashState == true) {
		return true;
	}

	if (canClimb && nearestClimbable == nullptr) nearestClimbable = prevClimbable;
	if (!isClimbing) 
	{
		UpdateAttack(dt);
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
			Attack();
		}
	}

	ZoneScoped;

	if (isHurt) {
		if (hurtTimer.ReadMSec() > 300.0f) {
			isHurt = false;
		}
	}

	floorSensorBody->SetPosition((int)position.getX(), (int)position.getY() + 60);
	if (hasWallJump)
	{
		if (wallSensorLeft == nullptr)
		{
			wallSensorLeft = Engine::GetInstance().physics->Func_CreateTemporarySensor(10, texH / 3, (int)position.getX() + texW / 6, (int)position.getY(), ColliderType::WALL_SENSOR);
			wallSensorLeft->listener = this;
		}
		if (wallSensorRight == nullptr)
		{
			wallSensorRight = Engine::GetInstance().physics->Func_CreateTemporarySensor(10, texH / 3, (int)position.getX() - texW / 6, (int)position.getY(), ColliderType::WALL_SENSOR);
			wallSensorRight->listener = this;
		}
		wallSensorLeft->SetPosition((int)position.getX() - texW / 4, (int)position.getY());
		wallSensorRight->SetPosition((int)position.getX() + texW / 4, (int)position.getY());
	}

	GetPhysicsValues();

	if (stepUpTimer > 0.0f)
	{
		stepUpTimer -= dt;
		if (stepUpTimer < 0.0f)
		{
			stepUpTimer = 0.0f;
		}
	}

	isSteppingUp = false;

	if (timerPasos > 0.0f) {
		timerPasos -= dt; // for audio
	}

	if (timerSuck > 0.0f) {
		timerSuck -= dt;
	}


	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		int shapeCount = b2Body_GetShapeCount(pbody->body);
		std::vector<b2ShapeId> shapes(shapeCount);
		b2Body_GetShapes(pbody->body, shapes.data(), shapeCount);

		godMode = !godMode;
		if (godMode) {
			b2Body_SetGravityScale(pbody->body, 0.0f);

			//Disable collisions
			b2Filter filter = b2Shape_GetFilter(shapes[0]);
			filter.maskBits = 0; // 0 significa que no colisionará con nada
			b2Shape_SetFilter(shapes[0], filter);
		}
		else {
			b2Body_SetGravityScale(pbody->body, gravityScale);

			//Enable collisions
			b2Filter filter = b2Shape_GetFilter(shapes[0]);
			filter.maskBits = 0xFFFFFFFF; // Restaura la máscara por defecto (colisiona con todo)
			b2Shape_SetFilter(shapes[0], filter);
			
		}

	}

	if (!isHurt) {
		if (!isClimbing) {
			if (isAdrenaline) {
				Func_BoostMovement();
			}
			else {
				Move();
			}
		}

		if (touchingMucose != nullptr) 
		{
			if (touchingMucose->state == 3) 
			{
				playerCurrentHp -= 1;
				isHurt = true;
				hurtTimer.Start();
			}
		}

		if (!isSmall) {
			AutoStepUp();

			Jump(dt);

			if (!isClimbing) {
				Func_Attacks(dt);
			}
		}
	}

	if (hasCrouch) {
		Func_Small();
	}
	Func_Climb();
	Func_PlayerState();
	Teleport();
	ApplyPhysics();


	if (IsGodMode())
	{
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
		{
			Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

			float scale = Engine::GetInstance().window->GetScale();
			float cameraX = Engine::GetInstance().render->camera.x;
			float cameraY = Engine::GetInstance().render->camera.y;

			float worldX = (mousePos.getX() - cameraX) / scale;
			float worldY = (mousePos.getY() - cameraY) / scale;

			LOG("--- DEBUG MOUSE ---");
			LOG("SCREEN -> X: %.2f | Y: %.2f", mousePos.getX(), mousePos.getY());
			LOG("WORLD  -> X: %.2f | Y: %.2f", worldX, worldY);
			LOG("-------------------");
		}
	}

	return true;
}

bool Player::TryStepUp()
{
	if (!onGround || isJumping || !isMoving || isSucking || isAttacking || stepUpTimer > 0.0f)
	{
		return false;
	}

	int playerX, playerY;
	pbody->GetPosition(playerX, playerY);

	int direction = facingRight ? 1 : -1;

	int bodyWidth = texW / 2;
	int bodyHeight = isSmall ? (texH - 50) / 2 : (texH - 50);
	int halfWidth = bodyWidth / 2;
	int halfHeight = bodyHeight / 2;

	int frontX = playerX + direction * (halfWidth + 6);
	int frontFeetY = playerY + halfHeight - 12;

	bool wallInFront = Engine::GetInstance().map->IsCollisionTileAtWorldPos(frontX, frontFeetY);

	if (!wallInFront)
	{
		return false;
	}

	for (int step = 4; step <= stepHeight; step += 4)
	{
		int testX = playerX + direction * stepForward;
		int testY = playerY - step;

		int left = testX - halfWidth + 6;
		int right = testX + halfWidth - 6;
		int top = testY - halfHeight + 6;
		int bottom = testY + halfHeight - 6;

		bool blocked = Engine::GetInstance().map->IsCollisionTileAtWorldPos(left, top) || Engine::GetInstance().map->IsCollisionTileAtWorldPos(right, top) || Engine::GetInstance().map->IsCollisionTileAtWorldPos(left, bottom) || Engine::GetInstance().map->IsCollisionTileAtWorldPos(right, bottom);

		if (!blocked)
		{
			pbody->SetPosition(testX, testY);

			velocity.y = 0.0f;
			onGround = true;
			isJumping = false;
			isHoldingJump = false;
			jumpHoldTime = 0.0f;
			isSteppingUp = true;
			stepUpTimer = stepUpCooldown;

			currentState = PLAYERSTATE::MOVE;
			anims.SetCurrent("run");

			return true;
		}
	}

	return false;
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

	isMoving = false; 

	int x_axis_raw = SDL_GetGamepadAxis(Engine::GetInstance().input->controller, SDL_GAMEPAD_AXIS_LEFTX);
	int y_axis_raw = SDL_GetGamepadAxis(Engine::GetInstance().input->controller, SDL_GAMEPAD_AXIS_LEFTY);

	float x_axis_norm = x_axis_raw / 32767.0f;
	float y_axis_norm = y_axis_raw / 32767.0f;

	if (x_axis_norm > -0.2f && x_axis_norm < 0.2f) {
		x_axis_norm = 0.0f;
	}
	if (y_axis_norm > -0.2f && y_axis_norm < 0.2f) {
		y_axis_norm = 0.0f;
	}

	float speedToUse = normalSpeed;
	if (touchingMucose != nullptr) speedToUse = mucoseSpeed;

	if (godMode) {
		velocity = { 0, 0 };

		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || x_axis_norm <= -0.1) {
			isMoving = true;
			velocity.x = -speedToUse * 2;
			facingRight = false;
		}
		else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || x_axis_norm >= 0.1) {
			isMoving = true;
			velocity.x = speedToUse * 2;
			facingRight = true;
		}

		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT || y_axis_norm <= -0.1) {
			isMoving = true;
			velocity.y = -speedToUse * 2;

		}
		else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT || y_axis_norm >= 0.1) {
			isMoving = true;
			velocity.y = speedToUse * 2;

		}
		Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
	}
	else if (isVulnerable)
	{
		if (dashing == false)
		{
			if ((Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || x_axis_norm <= -0.1) && !isSucking && canMove) {
				isMoving = true;
				velocity.x = -vulnerableSpeed;
				facingRight = false;
			}
			if ((Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || x_axis_norm >= 0.1) && !isSucking && canMove) {
				isMoving = true;
				velocity.x = vulnerableSpeed;
				facingRight = true;
			}
		}

		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT ||
			Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT ||
			x_axis_norm <= -0.1 || x_axis_norm >= 0.1)
		{
			isMoving = true;
		}
		else
		{
			isMoving = false;
		}
	}
	else {
		if (dashing == false)
		{
			if ((Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || x_axis_norm <= -0.1) && !isSucking && canMove) {
				isMoving = true;
				velocity.x = -speedToUse;
				facingRight = false;
			}
			if ((Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || x_axis_norm >= 0.1) && !isSucking && canMove) {
				isMoving = true;
				velocity.x = speedToUse;
				facingRight = true;
			}
		}

		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT ||
			Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || 
			x_axis_norm <= -0.1 || x_axis_norm >= 0.1)
		{
			isMoving = true;
		}
		else
		{
			isMoving = false;
		}
	}
	

	// CONTROL DEL AUDIO
	if (isMoving && onGround)
	{
		if (timerPasos <= 0.0f)
		{
			Engine::GetInstance().audio->PlayFx(walkFxId); 

			timerPasos = 500.0f;
		}
	}

	if (isSucking)
	{
		if (timerSuck <= 0.0f)
		{
			Engine::GetInstance().audio->PlayFx(suckAttackFxId);

			
			timerSuck = 500.0f;
		}
	}
	else
	{
		
		timerSuck = 0.0f;
	}

}

void Player::AutoStepUp() // STEP UP SMOOTHING ("TRYING TO AVOID THE "STAIR PROBLEM")
{
	if (isStepUpSmoothing)
	{
		float dt = Engine::GetInstance().GetDt();
		stepUpSmoothTimer += dt;

		float t = stepUpSmoothTimer / stepUpSmoothDuration;
		if (t > 1.0f) t = 1.0f;

		int newX = (int)(stepUpStartX + (stepUpTargetX - stepUpStartX) * t);
		int newY = (int)(stepUpStartY + (stepUpTargetY - stepUpStartY) * t);

		pbody->SetPosition(newX, newY);
		Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity.x, 0.0f);

		if (t >= 1.0f)
		{
			isStepUpSmoothing = false;
			isSteppingUp = true;
			stepUpTimer = stepUpCooldown;
			onGround = true;
			isJumping = false;
			velocity.y = 0.0f;
		}

		return;
	}

	if (!onGround || !isMoving || isSucking || isAttacking || stepUpTimer > 0.0f)
		return;

	int targetX = 0;
	int targetY = 0;

	if (!FindStepUpTarget(targetX, targetY))
		return;

	int playerX, playerY;
	pbody->GetPosition(playerX, playerY);

	stepUpStartX = playerX;
	stepUpStartY = playerY;
	stepUpTargetX = targetX;
	stepUpTargetY = targetY;

	stepUpSmoothTimer = 0.0f;
	isStepUpSmoothing = true;

	velocity.y = 0.0f;
	currentState = PLAYERSTATE::MOVE;
	anims.SetCurrent("run");
}

bool Player::FindStepUpTarget(int& targetX, int& targetY) // CHECK IF THERE'S A STEP AND GET THE TARGET POSITION TO STEP UP ("TRYING TO AVOID THE "STAIR PROBLEM")
{
	int playerX, playerY;
	pbody->GetPosition(playerX, playerY);

	int direction = facingRight ? 1 : -1;

	int bodyWidth = texW / 2;
	int bodyHeight = isSmall ? (texH - 50) / 2 : (texH - 50);

	int halfWidth = bodyWidth / 2;
	int halfHeight = bodyHeight / 2;

	int tileW = Engine::GetInstance().map->GetTileWidth();
	int tileH = Engine::GetInstance().map->GetTileHeight();

	int frontX = playerX + direction * (halfWidth + stepCheckDistance);
	int feetY = playerY + halfHeight - 4;

	bool foundWall = false;
	int wallY = feetY;

	for (int y = feetY; y >= feetY - maxStepHeight; y -= 4)
	{
		if (Engine::GetInstance().map->IsCollisionTileAtWorldPos(frontX, y))
		{
			foundWall = true;
			wallY = y;
			break;
		}
	}

	if (!foundWall)
		return false;

	Vector2D tile = Engine::GetInstance().map->WorldToMap(frontX, wallY);
	Vector2D tileWorld = Engine::GetInstance().map->MapToWorld((int)tile.getX(), (int)tile.getY());

	int tileTopY = (int)tileWorld.getY();

	targetY = tileTopY - halfHeight;

	// Empuja el centro un poco hacia dentro del tile para que no siga chocando con la pared.
	targetX = playerX + direction * 14;

	int left = targetX - halfWidth + 4;
	int right = targetX + halfWidth - 4;
	int top = targetY - halfHeight + 4;
	int bottom = targetY + halfHeight - 4;

	bool blocked =
		Engine::GetInstance().map->IsCollisionTileAtWorldPos(left, top) ||
		Engine::GetInstance().map->IsCollisionTileAtWorldPos(right, top) ||
		Engine::GetInstance().map->IsCollisionTileAtWorldPos(left, bottom) ||
		Engine::GetInstance().map->IsCollisionTileAtWorldPos(right, bottom);

	if (blocked)
		return false;

	int stepAmount = playerY - targetY;

	if (stepAmount <= 0 || stepAmount > maxStepHeight)
		return false;

	return true;
}

void Player::ActivateSpeedBoost() {
	isAdrenaline = true;
	boostTimer_01.Start();
	LOG("Boost iniciado!");
}

void Player::ActivateBerserker() {
	isBerserker = true;
	BerserkerTimer.Start();
}

void Player::ActivateAdrenaline() {
	isAdrenaline = true;
	AdrenalineTimer.Start();
	LOG("adrenalina iniciadsa");
}

void Player::RestoreHealthB()
{
	playerCurrentHp++;
	if (playerCurrentHp > playerMaxHp)
	{
		playerCurrentHp = playerMaxHp;
	}	
}

void Player::ActivateCamouflage() {
	isCamouflage = true;
	camouflageTimer.Start();
}

void Player::Func_BoostMovement() {
	float durationMS = 20000.0f;
	boostTimer_01.Start();
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
		isAdrenaline = false;
		LOG("Boost terminado");
	}
}

void Player::Jump(float dt)
{
	KeyState spaceState = Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE);
	
	/* This is for detecting if a controller button is pressed (because it doesnt have KEY_REPEAT)*/
	if (Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_EAST) == KEY_DOWN)
		controllerJumpState = true;
	else if (Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_EAST) == KEY_UP)
		controllerJumpState = false;

	if ((spaceState == KEY_DOWN || controllerJumpState) && !isJumping && !isSucking && ((onGround && canJump) || isClimbing || canWallJump))
	{
		Engine::GetInstance().audio->PlayFx(jumpFxId);
		currentState = PLAYERSTATE::PREPARE_JUMP;

		if (!onGround) 
		{
			wallJumpsLeft -= 1;
			velocity.y = 0;
			dashLeft += 1;
			isClimbing = false;
			b2Body_SetGravityScale(pbody->body, gravityScale);
			Engine::GetInstance().physics->SetLinearVelocity(pbody, b2Vec2_zero);
		}

		prevClimbable = nearestClimbable;

		float forceToUse = jumpForce;

		if (hasPowerJump == true)
		{
			forceToUse = powerJumpForce;
		}

		Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, 0.0f, -forceToUse, true);

		anims.SetCurrent("prepareJump");

		isJumping = true;
		onGround = false;
		isClimbing = false;

		isHoldingJump = true;
		jumpHoldTime = 0.0f;
	}
	
	if ((spaceState == KEY_DOWN || spaceState == KEY_REPEAT || controllerJumpState) && isHoldingJump && isJumping)
	{
		if (jumpHoldTime < maxJumpHoldTime)
		{
			Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, 0.0f, -extraJumpForce, true);
			jumpHoldTime += dt;
		}
	}

	if (spaceState == KEY_UP)
	{
		isHoldingJump = false;
	}
}

void Player::Func_PlayerState() {
	
	if (currentState == PLAYERSTATE::DEATH) {
		if (anims.HasCurrentAnimationFinished() || (deathTimer.ReadMSec() > 700 && !dead)) 
		{
			//Destroy();
			dead = true;
			Engine::GetInstance().scene->ActivateGameOver();
		}
		return;
	}

	if (isSteppingUp)
	{
		if (isMoving)
		{
			currentState = PLAYERSTATE::MOVE;
			anims.SetCurrent("run");
		}
		else
		{
			currentState = PLAYERSTATE::IDLE;
			anims.SetCurrent("idle");
		}

		isJumping = false;
		isHoldingJump = false;
		jumpHoldTime = 0.0f;
		onGround = true;

		return;
	}

	if (currentState == PLAYERSTATE::JUMP_CASCADA ||
		currentState == PLAYERSTATE::INSIDE_CASCADA ||
		currentState == PLAYERSTATE::OUT_CASCADA)
	{
		switch (currentState)
		{
		case PLAYERSTATE::JUMP_CASCADA:
			if (cascadaTimer.ReadMSec() >= 1400.0f)
			{
				currentState = PLAYERSTATE::INSIDE_CASCADA;
				anims.SetCurrent("insideCascada");
			}
			break;

		case PLAYERSTATE::INSIDE_CASCADA:
			break;

		case PLAYERSTATE::OUT_CASCADA:
			if (anims.HasCurrentAnimationFinished())
			{
				currentState = PLAYERSTATE::IDLE;
				anims.SetCurrent("idle");
			}
			break;
		}

		return;
	}

	if ((currentState == PLAYERSTATE::FALLING_JUMP ||
		currentState == PLAYERSTATE::JUMPING ||
		currentState == PLAYERSTATE::PREPARE_JUMP)
		&& onGround && !isAttacking)
	{
		Engine::GetInstance().audio->PlayFx(caerFxId);
		currentState = PLAYERSTATE::END_JUMP;
		anims.SetCurrent("endJump");

		isJumping = false;
		isHoldingJump = false;
		jumpHoldTime = 0.0f;
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

	case Player::PLAYERSTATE::CLIMB:

		if ((Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) || (Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) && dashState == false && nearestClimbable != nullptr){
			anims.SetCurrent("climb");
	    }

		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_DOWN ||
			Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			// Al pulsar A, miramos a la izquierda
			facingRight = false;
		}
		else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_DOWN ||
			Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			// Al pulsar D, miramos a la derecha
			facingRight = true;
		}
	default:
		break;
	}

	float durationBerserker = 18000.0f; 
	float durationVulnerable = 5000.0f; 

	if (isBerserker)
	{
		if (BerserkerTimer.ReadMSec() > durationBerserker)
		{
			isBerserker = false;
		}
	}

	if (isVulnerable)
	{
		if (vulnerableTimer.ReadMSec() > durationVulnerable)
		{
			isVulnerable = false;
		}
	}
}

void Player::Func_Attacks(float dt) {
	// Stun ATTACK
	// Left click attack
	bool wantsToAttack = Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_WEST) == KEY_DOWN;
	if (wantsToAttack && !isAttacking && !isSucking && canAttack)
	{
		Engine::GetInstance().audio->PlayFx(stunAttackFxId);
		Engine::GetInstance().audio->PlayFx(stunAttackDocFxId);

		currentState = PLAYERSTATE::ATTACK;
		isAttacking = true;

		if (!onGround)
		{
			attackTimer = 750.0f;          
			anims.SetCurrent("airAttack");
		}
		else
		{
			attackTimer = 750.0f;
			anims.SetCurrent("stun");
		}
	}

	if (isAttacking) {
		attackTimer -= dt;

		if (syringeBody != nullptr) {
			Engine::GetInstance().physics->DeletePhysBody(syringeBody);
			syringeBody = nullptr;
		}

		if (attackTimer <= 100.0f && attackTimer > 0.0f) {

			float progress = 1.0f - (attackTimer / 100.0f);

			float width, height;
			float pivotLocalX, pivotLocalY;

			if (!onGround)
			{
				width = 150.0f;
				height = 170.0f;
				pivotLocalX = 30.0f;
				pivotLocalY = -30.0f;
			}
			else
			{
				width = 110.0f;
				height = 140.0f;
				pivotLocalX = 80.0f;
				pivotLocalY = 0.0f;

			}
			
			float startAngle = 90.0f;
			float endAngle = facingRight ? -90.0f : 270.0f;

			if (!facingRight) {
				pivotLocalX = -pivotLocalX;
			}

			int playerX, playerY;
			pbody->GetPosition(playerX, playerY);
			float distX_player = playerX + pivotLocalX;
			float distY_player = playerY + pivotLocalY;

			syringeBody = Engine::GetInstance().physics->Func_CreateTemporarySensor(
				(int)width, (int)height, distX_player, distY_player, ColliderType::SYRINGE);
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

	// Suck ATTACK
	if (Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_NORTH) == KEY_DOWN)
		controllerSuckState = true;
	else if (Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_NORTH) == KEY_UP)
		controllerSuckState = false;

	if ((Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT || controllerSuckState) && !isAttacking && !isSucking && canAttack && onGround) {
		currentState = PLAYERSTATE::SUCKING;
		isSucking = true;
		anims.SetCurrent("extract");

		int playerX, playerY;
		pbody->GetPosition(playerX, playerY);

		float width = 110.0f;
		float height = 90.0f;
		float pivotLocalX = facingRight ? 78.5f : -78.5f;

		suckBody = Engine::GetInstance().physics->Func_CreateTemporarySensor((int)width, (int)height, pivotLocalX, playerY, ColliderType::SUCK_ZONE, 0.0f);

		suckBody->listener = this;
	}

	if (isSucking) {


		if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_UP || (!controllerSuckState && Engine::GetInstance().input->controller != NULL)) {
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
				float pivotLocalX = facingRight ? 78.5f : -78.5f;
				suckBody->SetPosition((int)(playerX + pivotLocalX), playerY + 22);
			}
		}
	}
}

void Player::Func_Small() {
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN && onGround) {
		Engine::GetInstance().physics->DeletePhysBody(pbody);
		pbody = Engine::GetInstance().physics->CreateRectangle((int)position.getX(), (int)position.getY() + 50, texW / 2, (texH - 50) / 2, bodyType::DYNAMIC);
		pbody->SetFixedRotation(true);
		pbody->listener = this;
		pbody->ctype = ColliderType::PLAYER;
		isSmall = true;
	}
	else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP && isSmall && onGround) {
		Engine::GetInstance().physics->DeletePhysBody(pbody);
		pbody = Engine::GetInstance().physics->CreateRectangle((int)position.getX(), (int)position.getY() - 25, texW / 2, texH - 50, bodyType::DYNAMIC);

		pbody->SetFixedRotation(true);
		pbody->listener = this;

		pbody->ctype = ColliderType::PLAYER;
		isSmall = false;
	}
}

void Player::Func_Dash()
{
	if ((Engine::GetInstance().input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) && dashState == false)
	{
		dashState = true;
		dashPos = GetPosition();
	}
	else if (dashState == true)
	{
		b2Body_SetGravityScale(pbody->body, 0.0f);
		SetPosition(dashPos);
		bool dash = false;
		Vector2D dashDir;
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
		{
			dashDir = Vector2D(dashDir.getX(), -1);
			dash = true;
		}
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_DOWN) 
		{
			dashDir = Vector2D(-1, dashDir.getY());
			facingRight = false;
			dash = true;
		}
		else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) 
		{
			dashDir = Vector2D(1, dashDir.getY());
			facingRight = true;
			dash = true;
		}
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
		{
			dashDir = Vector2D(dashDir.getX(), -1);
			dash = true;
		}
		if (dash == true) {
			dashState = false;
			Vector2D dashForce = Vector2D (dashDir.getX() * 30, dashDir.getY() * 15);
			Engine::GetInstance().physics->SetLinearVelocity(pbody, b2Vec2_zero);
			Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, dashForce.getX(), dashForce.getY());
			if (dashDir.getY() == 0) 
			{
				b2Body_SetGravityScale(pbody->body, 0.0f);
			}
			Engine::GetInstance().audio->PlayFx(dashFxId);
			dashing = true;
			dashLeft -= 1;
			dashTimer.Start();
		}
	}
}
//
void Player::Func_Climb()
{
	if ((Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) && dashState == false && nearestClimbable != nullptr)
	{
		isJumping = false;
		if (!isClimbing)
		{
			Engine::GetInstance().physics->SetLinearVelocity(pbody, b2Vec2_zero);
			b2Body_SetGravityScale(pbody->body, 0.0f);
			isClimbing = true;

			if (nearestClimbable != nullptr && nearestClimbable->isWaterfall)
			{
				currentState = PLAYERSTATE::JUMP_CASCADA;
				anims.SetCurrent("jumpCascada");
				cascadaTimer.Start();
			}
			else
			{
				currentState = PLAYERSTATE::CLIMB;
			}
		}
		velocity.y = -7;

		if (nearestClimbable != nullptr &&
			nearestClimbable->isWaterfall &&
			currentState == PLAYERSTATE::INSIDE_CASCADA)
		{
			anims.SetCurrent("insideCascada");
		}
	}
	if ((Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) && dashState == false && nearestClimbable != nullptr)
	{
		isJumping = false;
		if (!onGround) {
			if (!isClimbing && Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
			{
				Engine::GetInstance().physics->SetLinearVelocity(pbody, b2Vec2_zero);
				b2Body_SetGravityScale(pbody->body, 0.0f);
				isClimbing = true;

				if (nearestClimbable != nullptr && nearestClimbable->isWaterfall)
				{
					currentState = PLAYERSTATE::JUMP_CASCADA;
					anims.SetCurrent("jumpCascada");
					cascadaTimer.Start();
				}
				else
				{
					currentState = PLAYERSTATE::CLIMB;
				}
			}
			velocity.y = 7;

			if (nearestClimbable != nullptr &&
				nearestClimbable->isWaterfall &&
				currentState == PLAYERSTATE::INSIDE_CASCADA)
			{
				anims.SetCurrent("insideCascada");
			}
		}
		else {
			currentState = PLAYERSTATE::IDLE;
			isClimbing = false;
			b2Body_SetGravityScale(pbody->body, gravityScale);
		}
	}
	if ((Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_UP || Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_UP) && isClimbing == true)
	{
		velocity.y = 0;
	}
}

void Player::ApplyPhysics() {
	// Preserve vertical speed while jumping
	if (isJumping == true && isClimbing == false) {
		velocity.y = Engine::GetInstance().physics->GetYVelocity(pbody);
	}

	if (dashing == true) {
		velocity.y = Engine::GetInstance().physics->GetYVelocity(pbody);
		velocity.x = Engine::GetInstance().physics->GetXVelocity(pbody);
	}
	// Apply velocity via helper

	if (velocity.y > 0 && velocity.y <= 17.7f) 
	{
		velocity.y += 0.3f;
	}
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void Player::Draw(float dt) {
	if ((isClimbing && (
		Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_DOWN ||
		Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_DOWN ||
		Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT ||
		Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT
		)) ||
		isClimbing == false ||
		currentState == PLAYERSTATE::JUMP_CASCADA ||
		currentState == PLAYERSTATE::INSIDE_CASCADA ||
		currentState == PLAYERSTATE::OUT_CASCADA)
	{
		anims.Update(dt);
	}
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
	
	if (isCamouflage) {
		SDL_SetTextureAlphaMod(texture, 128); 
	}
	else {
		SDL_SetTextureAlphaMod(texture, 255);
	}

	float drawOffsetX = facingRight ? -300.0f : -210.0f;
	float drawOffsetY = -450.0f;

	if (currentState == PLAYERSTATE::JUMP_CASCADA ||
		currentState == PLAYERSTATE::INSIDE_CASCADA ||
		currentState == PLAYERSTATE::OUT_CASCADA)
	{
		drawOffsetX = -330.0f;
		drawOffsetY = -200.0f;
	}

	if (facingRight)
	{
		Engine::GetInstance().render->DrawTexture(texture, position.getX() + drawOffsetX, position.getY() + drawOffsetY, &animFrame, 1.0f, 0.0, texW / 2, texH / 2, SDL_FLIP_NONE, 1.0f);
		
	}
	else
	{
		Engine::GetInstance().render->DrawTexture(texture, position.getX() + drawOffsetX, position.getY() + drawOffsetY, &animFrame, 1.0f, 0.0, texW / 2, texH / 2, SDL_FLIP_HORIZONTAL, 1.0f);
	}
	
	SDL_SetTextureAlphaMod(texture, 255);

	if (effectAnims.HasCurrentAnimationFinished() == true) {
		healing = false;
	}

	if (healing == true) {
		effectAnims.Update(dt);
		const SDL_Rect& effectAnimFrame = effectAnims.GetCurrentFrame();
		if (facingRight)
		{
			Engine::GetInstance().render->DrawTexture(healText, position.getX() - 258, position.getY() - 170, &effectAnimFrame, 1.0f, 0.0, texW / 2, texH / 2, SDL_FLIP_NONE, 1.0f);
		}
		else
		{
			Engine::GetInstance().render->DrawTexture(healText, position.getX() - 258, position.getY() - 170, &effectAnimFrame, 1.0f, 0.0, texW / 2, texH / 2, SDL_FLIP_HORIZONTAL, 1.0f);
		}
	}
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures->UnLoad(texture);

	if (pbody != nullptr)
	{
		Engine::GetInstance().physics->DeletePhysBody(pbody);
		Engine::GetInstance().physics->DeletePhysBody(floorSensorBody);
		Engine::GetInstance().physics->DeletePhysBody(wallSensorLeft);
		Engine::GetInstance().physics->DeletePhysBody(wallSensorRight);
		floorSensorBody = nullptr;
		wallSensorLeft = nullptr;
		wallSensorRight = nullptr;
		pbody = nullptr;
	}

	return true;
}
 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	Enemy* enemy;

	if (godMode)
		return;

	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:

		if (physA->ctype == ColliderType::SENSOR)
		{
			groundContacts++;
			onGround = true;
			isJumping = false;
			wallJumpsLeft = 1;
			dashLeft = 1;
		}

		if (onGround == false && physA->ctype == ColliderType::WALL_SENSOR)
		{
			canWallJump = true;
			isJumping = false;
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
	case ColliderType::DOOR:
		isClimbing = false;
		canClimb = false;
		nearestClimbable = nullptr;
		b2Body_SetGravityScale(pbody->body, gravityScale);
		anims.SetCurrent("idle");
		break;
	case ColliderType::CELL_ATTACK:
	{
		if (!godMode && currentState != PLAYERSTATE::DEATH)
		{
			playerCurrentHp -= 1.0f;

			if (playerCurrentHp < 0)
				playerCurrentHp = 0;

			Engine::GetInstance().audio->PlayFx(hurtFxId);

			if (playerCurrentHp > 0)
			{
				isHurt = true;
				hurtTimer.Start();
			}
			else
			{
				Engine::GetInstance().audio->PlayFx(deathFxId);
				Engine::GetInstance().audio->PlayFx(UIdeathID, 3);
				canMove = false;
				canJump = false;
				canAttack = false;
				currentState = PLAYERSTATE::DEATH;
				anims.SetCurrent("death");
				deathTimer.Start();
			}
		}

		break;
	}
	case ColliderType::ENEMY:{
		LOG("End Collision ENEMY");

		Entity* entityPtr = (Entity*)physB->listener;

		if (!godMode && !isHurt && currentState != PLAYERSTATE::DEATH && !entityPtr->IsEnemyStunned())
		{
			if (isBerserker)
			{
				playerCurrentHp = playerCurrentHp - 0.5;
				LOG("Current HP: %f", playerCurrentHp);
			}
			else if (isVulnerable)
			{
				playerCurrentHp = playerCurrentHp - 2;
				LOG("Current HP: %f", playerCurrentHp);
			}
			else
			{
				playerCurrentHp--;
				LOG("Current HP: %f", playerCurrentHp);
			}
			Engine::GetInstance().audio->PlayFx(hurtFxId);
			b2Body_SetGravityScale(pbody->body, gravityScale);
			isClimbing = false;
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
				Engine::GetInstance().audio->PlayFx(deathFxId);
				Engine::GetInstance().audio->PlayFx(UIdeathID, 2);
				LOG("Player has died!");
				canMove = false;
				canJump = false;
				canAttack = false;
				currentState = PLAYERSTATE::DEATH;
				anims.SetCurrent("death");
				deathTimer.Start();
			}
		}
		break;
	}
	case ColliderType::BOSS_R_HAND: {
		LOG("End Collision BOSS_R_HAND");

		Entity* entityPtr = (Entity*)physB->listener;

		if (!godMode && !isHurt && currentState != PLAYERSTATE::DEATH && !entityPtr->IsEnemyStunned())
		{
			playerCurrentHp--;
			Engine::GetInstance().audio->PlayFx(hurtFxId);
			b2Body_SetGravityScale(pbody->body, gravityScale);
			isClimbing = false;
			LOG("Current HP: %i", playerCurrentHp);

			if (playerCurrentHp > 0)
			{
				Engine::GetInstance().audio->PlayFx(hurtFxId);
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
				Engine::GetInstance().audio->PlayFx(deathFxId);
				Engine::GetInstance().audio->PlayFx(UIdeathID,2);
				LOG("Player has died!");
				canMove = false;
				canJump = false;
				canAttack = false;
				currentState = PLAYERSTATE::DEATH;
				anims.SetCurrent("death");
			}
		}
		break;
	}
	case ColliderType::BOSS_L_HAND: {
		LOG("End Collision BOSS_L_HAND");

		Entity* entityPtr = (Entity*)physB->listener;

		if (!godMode && !isHurt && currentState != PLAYERSTATE::DEATH && !entityPtr->IsEnemyStunned())
		{
			playerCurrentHp--;
			Engine::GetInstance().audio->PlayFx(hurtFxId);
			b2Body_SetGravityScale(pbody->body, gravityScale);
			isClimbing = false;
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
				Engine::GetInstance().audio->PlayFx(deathFxId);
				Engine::GetInstance().audio->PlayFx(UIdeathID, 2);
				LOG("Player has died!");
				canMove = false;
				canJump = false;
				canAttack = false;
				currentState = PLAYERSTATE::DEATH;
				anims.SetCurrent("death");
			}
		}
		break;
	}
	case ColliderType::CHECKPOINT:
		LOG("Collision CHECKPOINT");
		break;
	case ColliderType::CLIMBABLE: {
		Climbable* climbablePtr = (Climbable*)physB->listener;
		if (climbablePtr->isWaterfall && !hasAscend) {
			canClimb = false;
			nearestClimbable = nullptr;
			break;
		}
		else {
			canClimb = true;
			nearestClimbable = climbablePtr;
		}
		break;
	}
	case ColliderType::ACID:
		if (hasAcidResistance) break;
		Engine::GetInstance().audio->PlayFx(deathFxId);
		Engine::GetInstance().audio->PlayFx(UIdeathID, 2);
		LOG("Player has died!");
		canMove = false;
		canJump = false;
		canAttack = false;
		playerCurrentHp = 0;
		currentState = PLAYERSTATE::DEATH;
		anims.SetCurrent("death");
		break;
	case ColliderType::MUCOSA: {
		Mucosa* mucosa = (Mucosa*)physB->listener;
		mucosa->player = this;
		touchingMucose = mucosa;
		break;
	}
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		//LOG("End Collision PLATFORM");

		if (groundContacts > 0)
		{
			groundContacts--;
		}

		if (physA->ctype == ColliderType::SENSOR)
		{
			if (groundContacts > 0)
			{
				groundContacts--;
			}

			onGround = groundContacts > 0 || isSteppingUp;
		}

		if (physA->ctype == ColliderType::WALL_SENSOR)
		{
			canWallJump = false;
		}
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	case ColliderType::CHECKPOINT:
		LOG("End Collision CHECKPOINT");
		break;
	case ColliderType::CLIMBABLE:
		LOG("End Collision CLIMBABLE");
		if (isClimbing == false || isJumping) {
			nearestClimbable = nullptr;
			canClimb = false;
		}
		if (nearestClimbable != nullptr && nearestClimbable->isWaterfall)
		{
			currentState = PLAYERSTATE::OUT_CASCADA;
			anims.SetCurrent("outCascada");
		}
		b2Body_SetGravityScale(pbody->body, gravityScale);
		isClimbing = false;
		break;
	case ColliderType::MUCOSA:
		if (touchingMucose != nullptr) touchingMucose = nullptr;
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
	position = pos;

	if (pbody!= nullptr)
	{
		pbody->SetPosition((int)(pos.getX() + texW / 2), (int)(pos.getY() + texH / 2));
	}
}

void Player::SetRespawnPosition(Vector2D pos)
{
	respawnPosition = pos;
	LOG("Checkpoint reached! New respawn: %f, %f", pos.getX(), pos.getY());
}

void Player::StopMovement() {
	velocity.x = 0;
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

bool Player::Destroy()
{
	LOG("Destroying Player");
	active = false;
	pendingToDelete = true;
	return true;
}
