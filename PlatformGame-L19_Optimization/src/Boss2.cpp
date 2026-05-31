#include "Boss2.h"
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

Boss2::Boss2() : Entity(EntityType::BOSS2)
{
	name = "Boss2";
}

Boss2::~Boss2() {

}

bool Boss2::Awake() {
	return true;
}

bool Boss2::Start() {
	//Load All Animations
	anim_intro = new bossAnimation(24, "intro", false);
	for (int i = 1; i <= anim_intro->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss2/Intro/Intro_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		anim_intro->animation.push_back(frame);
	}
	animations.push_back(&anim_intro->animation);

	triggerBody = Engine::GetInstance().physics->CreateRectangleSensor(position.getX() + 960, position.getY() + 960, 1920, 1920, bodyType::STATIC);
	triggerBody->ctype = ColliderType::SENSOR;
	triggerBody->listener = this;

	return true;
}

bool Boss2::Update(float dt)
{
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_B) == KEY_DOWN && !active) {
		Initialize();
	}
	if (!active) return true;
	ZoneScoped;

	GetPhysicsValues();

	//Func_EnemyStates(dt);
	ApplyPhysics();
	Draw(dt);

	return true;
}

void Boss2::GetPhysicsValues() {
	return;
}

void Boss2::ApplyPhysics() {

	//Apply velocity via helper
	return;
}

void Boss2::Draw(float dt)
{
	//Draw Body
	totalAnimFrames = currentAnimation->frames;
	if (frameTimer.ReadMSec() >= 50)
	{
		frameTimer.Start();
		//Get current body frame
		if (currentFrame < totalAnimFrames) {
			currentFrame += 1;
		}
		else if (currentAnimation->loop == true) {
			currentFrame = 1;
		}
		else {
			AnimationFinished(currentAnimation);
		}
	}
	Engine::GetInstance().render->DrawTexture(currentAnimation->animation.at(currentFrame - 1), position.getX(), position.getY(), NULL);
}

bool Boss2::CleanUp()
{
	LOG("Cleanup enemy");
	for (int i = 0; i < animations.size(); ++i) {
		for (int j = 0; j < animations.at(i)->size(); ++j) {
			Engine::GetInstance().textures->UnLoad(animations.at(i)->at(j));
		}
	}
	if (pbody != nullptr)
	{
		Engine::GetInstance().physics->DeletePhysBody(pbody);
	}
	return true;
}

bool Boss2::Destroy()
{
	LOG("Destroying Boss2");
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

bool Boss2::Destroy(Player* pplayer) // Good: coincide with the .h
{
	/*if (player->playerCurrentHp < player->playerMaxHp) {
		player->playerCurrentHp += 1;
	}
	player->healing = true;
	player->effectAnims.SetCurrent("lifeUp");*/
	return Destroy();
}

void Boss2::SetPosition(Vector2D pos) {
	pbody->SetPosition((int)(pos.getX()), (int)(pos.getY()));
}

Vector2D Boss2::GetPosition() {
	int x, y;
	pbody->GetPosition(x, y);
	// Adjust for center
	return Vector2D((float)x, (float)y);
}

bool Boss2::IsEnemyStunned() {

	if (currentState == BOSS_STATES::STUNED or currentState == BOSS_STATES::DEATH)
	{
		return true;
	}
	else return false;
}


//Define OnCollision function for the enemy. 
void Boss2::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		if (physA == triggerBody && position != Vector2D(0, 0)) {
			Initialize();
			Engine::GetInstance().physics->DeletePhysBody(triggerBody);
		}
		break;
	case ColliderType::SYRINGE:
		if (life > 0) {
			life -= 1;
			PlayAnimation(anim_hit);
		}
		break;
	default:
		break;
	}
}

void Boss2::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::SUCK_ZONE:
		break;
	}
}

void Boss2::AnimationFinished(bossAnimation* animation)
{
	return;
}

void Boss2::PlayAnimation(bossAnimation* animation)
{
	currentFrame = 1;
	currentAnimation = animation;
}

Boss2::bossAnimation::bossAnimation(int frames, std::string name, bool loop) {
	this->frames = frames;
	this->name = name;
	this->loop = loop;
}

void Boss2::Initialize() {
	initialPos = Vector2D(position.getX() + 1000, position.getY() + 850);
	//Set Current Animation as intro
	currentAnimation = anim_intro;
	frameTimer.Start();
	active = true;
}

void Boss2::Attack() {
	int attack = SDL_rand(2);
	return;
}