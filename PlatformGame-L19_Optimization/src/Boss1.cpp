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

	//Set Current Animation as intro
	currentBodyAnimation = intro_body;
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

	// Apply velocity via helper
	//Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void Boss1::Draw(float dt)
{
	//Draw Body
	totalBodyAnimFrames = currentBodyAnimation->frames;
	if (frameTimer.ReadMSec() >= 50) 
	{
		frameTimer.Start();
		if (currentBodyFrame < totalBodyAnimFrames) {
			currentBodyFrame += 1;
		}
		else if (currentBodyAnimation->loop == true) {
			currentBodyFrame = 1;
		}
		else {
			AnimationFinished(currentBodyAnimation);
		}
	}
	Engine::GetInstance().render->DrawTexture(currentBodyAnimation->animation.at(currentBodyFrame - 1), position.getX(), position.getY(), NULL);
}

bool Boss1::CleanUp()
{
	LOG("Cleanup enemy");
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
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
	pbody->SetPosition((int)(pos.getX()), (int)(pos.getY()));
}

Vector2D Boss1::GetPosition() {
	int x, y;
	pbody->GetPosition(x, y);
	// Adjust for center
	return Vector2D((float)x - texW / 2, (float)y - texH / 2);
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
		if (!isStunned)
		{
			timer_01.Start();
			currentState = BOSS_STATES::STUNED;
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

void Boss1::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::SUCK_ZONE:
		// Stop sucking if player releases right click
		isBeingSucked = false;
		break;
	}
}

void Boss1::AnimationFinished(bossAnimation* animation)
{
	if (animation->name == "intro") {
		PlayAnimation(idle_body);
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