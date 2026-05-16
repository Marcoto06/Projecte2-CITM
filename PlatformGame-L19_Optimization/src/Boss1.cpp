#include "Boss1.h"
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

Boss1::Boss1() : Entity(EntityType::BOSS1)
{
	name = "Boss1";
}

Boss1::~Boss1() {

}

bool Boss1::Awake() {
	return true;
}

bool Boss1::Start() {

	initialHeadPos = Vector2D(position.getX() + 1000, position.getY() + 850);
	stunHeadPos = initialHeadPos + Vector2D(-340, 680);

	//idleRHandPos = initialHeadPos + Vector2D(-200, 200);
	idleRHandPos = initialHeadPos;
	R_hand_body = Engine::GetInstance().physics->CreateRectangleSensor(idleRHandPos.getX(), idleRHandPos.getY() + 1000, 100, 100, bodyType::KINEMATIC);
	b2Body_SetGravityScale(R_hand_body->body, 0.0f);

	//Load All Animations
	idle_body = new bossAnimation(32, "idle", Body_Parts::BODY, true);
	for (int i = 1; i <= idle_body->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/IDLE/IDLE_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		idle_body->animation.push_back(frame);
	}

	intro_body = new bossAnimation(60, "intro", Body_Parts::BODY, false);
	for (int i = 1; i <= intro_body->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/INTRO/INTRO_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		intro_body->animation.push_back(frame);
	}

	hurt_body = new bossAnimation(16, "hurt", Body_Parts::BODY, false);
	for (int i = 1; i <= hurt_body->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/B.HURT/B.HURT_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		hurt_body->animation.push_back(frame);
	}

	stun_body = new bossAnimation(28, "stun", Body_Parts::BODY, false);
	for (int i = 1; i <= stun_body->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/STUN/STUN_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		stun_body->animation.push_back(frame);
	}

	//HAND ANIMATIONS

	intro_R_hand = new bossAnimation(1, "intro", Body_Parts::R_HAND, true);
	{
		SDL_Texture* frame = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Bosses/Boss1/RightHand/IDLE/IDLE_SS_1.png");
		intro_R_hand->animation.push_back(frame);
	}


	idle_R_hand = new bossAnimation(18, "idle", Body_Parts::R_HAND, true);
	for (int i = 1; i <= idle_R_hand->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/RightHand/IDLE/IDLE_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		idle_R_hand->animation.push_back(frame);
	}

	//Set Current Animation as intro
	currentBodyAnimation = intro_body;
	currentRHandAnimation = intro_R_hand;
	//Engine::GetInstance().physics->ApplyLinearImpulseToCenter(R_hand_body, 0, -10);
	frameTimer.Start();

	return true;
}

bool Boss1::Update(float dt)
{
	ZoneScoped;

	GetPhysicsValues();

	//Func_EnemyStates(dt);
	ApplyPhysics();
	Draw(dt);

	return true;
}

void Boss1::GetPhysicsValues() {
	return;
}

//void Boss1::Func_EnemyStates(float dt)
//{
//	switch (currentState)
//	{
//	case BOSS_STATES::WALKING:
//		anims.SetCurrent("idle");
//		Move();
//		break;
//
//	case BOSS_STATES::CHASING:
//		anims.SetCurrent("walk");
//		Move();
//		break;
//
//	case BOSS_STATES::STUNED:
//		anims.SetCurrent("stunned");
//
//		if (isBeingSucked)
//		{
//			if (!player->isAdrenaline)
//			{
//				if (suckTimer.ReadMSec() >= 3000.0f)
//				{
//					currentState = BOSS_STATES::DEATH;
//					return;
//
//				}
//			}
//			else
//			{
//				if (suckTimer.ReadMSec() >= 1500.0f)
//				{
//					currentState = BOSS_STATES::DEATH;
//					return;
//
//				}
//			}
//		}
//		else
//		{
//			if (timer_01.ReadMSec() > 7000.0f)
//			{
//				currentState = BOSS_STATES::WALKING;
//				isStunned = false;
//			}
//		}
//		break;
//
//	case BOSS_STATES::DEATH:
//
//		anims.SetCurrent("death");
//		if (anims.Func_HasCurrentAnimationFinished())
//		{
//			Destroy(attackingPlayer);
//			return;
//		}
//		break;
//	default:
//		break;
//	}
//}

void Boss1::ApplyPhysics() {

	//Apply velocity via helper
	Engine::GetInstance().physics->SetLinearVelocity(R_hand_body, R_hand_velocity);
}

void Boss1::Draw(float dt)
{
	//Draw Body
	totalBodyAnimFrames = currentBodyAnimation->frames;
	totalRHandAnimFrames = currentRHandAnimation->frames;
	if (frameTimer.ReadMSec() >= 50) 
	{
		frameTimer.Start();
		//Get current body frame
		if (currentBodyFrame < totalBodyAnimFrames) {
			currentBodyFrame += 1;
		}
		else if (currentBodyAnimation->loop == true) {
			currentBodyFrame = 1;
		}
		else {
			AnimationFinished(currentBodyAnimation);
		}
		//Get current hand frame
		if (currentRHandFrame < totalRHandAnimFrames) {
			currentRHandFrame += 1;
		}
		else if (currentBodyAnimation->loop == true) {
			currentRHandFrame = 1;
		}
		else {
			AnimationFinished(currentRHandAnimation);
		}
	}
	Engine::GetInstance().render->DrawTexture(currentBodyAnimation->animation.at(currentBodyFrame - 1), position.getX(), position.getY(), NULL);
	int r_hand_x, r_hand_y;
	R_hand_body->GetPosition(r_hand_x, r_hand_y);
	Engine::GetInstance().render->DrawTexture(currentRHandAnimation->animation.at(currentRHandFrame - 1), r_hand_x, r_hand_y, NULL);
}

bool Boss1::CleanUp()
{
	LOG("Cleanup enemy");
	//Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(head_body);
	return true;
}

bool Boss1::Destroy()
{
	LOG("Destroying Boss1");
	active = false;

	if (this->tiledId != -1) {
		Engine::GetInstance().scene->destroyedEntitiesIds.push_back(this->tiledId);
	}

	pendingToDelete = true;
	return true;
}

bool Boss1::Destroy(Player* pplayer) // Good: coincide with the .h
{
	if (player->playerCurrentHp < player->playerMaxHp) {
		player->playerCurrentHp += 1;
	}
	player->healing = true;
	player->effectAnims.SetCurrent("lifeUp");
	return Destroy();
}

void Boss1::SetPosition(Vector2D pos) {
	head_body->SetPosition((int)(pos.getX()), (int)(pos.getY()));
}

Vector2D Boss1::GetPosition() {
	int x, y;
	head_body->GetPosition(x, y);
	// Adjust for center
	return Vector2D((float)x, (float)y);
}

bool Boss1::IsEnemyStunned() {

	if (currentState == BOSS_STATES::STUNED or currentState == BOSS_STATES::DEATH)
	{
		return true;
	}
	else return false;
}


//Define OnCollision function for the enemy. 
void Boss1::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::SYRINGE:
		if (life > 0) {
			life -= 1;
			switch (physA->ctype) {
			case ColliderType::BOSS_HEAD:
				PlayAnimation(hurt_body);
				break;
			}
		}
		break;
	default:
		break;
	}
}

void Boss1::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::SUCK_ZONE:
		break;
	}
}

void Boss1::AnimationFinished(bossAnimation* animation)
{
	if (animation->name == "intro") {
		head_body = Engine::GetInstance().physics->CreateRectangleSensor(0, 0, 200, 300, ::STATIC);
		SetPosition(initialHeadPos);
		head_body->ctype = ColliderType::BOSS_HEAD;
		head_body->listener = this;
		PlayAnimation(idle_body);
	}
	else if (animation->name == "hurt") {
		if (life > 0) {
			switch (animation->part) {
			case Body_Parts::BODY:
				PlayAnimation(idle_body);
			}
		}
		else {
			Engine::GetInstance().physics->DeletePhysBody(head_body);
			PlayAnimation(stun_body);
		}
	}
	else if (animation->name == "stun") {
		head_body = Engine::GetInstance().physics->CreateRectangleSensor(0, 0, 150, 100, ::STATIC);
		SetPosition(stunHeadPos);
		head_body->ctype = ColliderType::BOSS_HEAD;
		head_body->listener = this;
	}
	return;
}

void Boss1::PlayAnimation(bossAnimation* animation)
{
	if (animation->part == Body_Parts::BODY)
	{
		currentBodyFrame = 1;
		currentBodyAnimation = animation;
		frameTimer.Start();
	}
}

Boss1::bossAnimation::bossAnimation(int frames, std::string name, Body_Parts part, bool loop) {
	this->frames = frames;
	this->name = name;
	this->part = part;
	this->loop = loop;
}