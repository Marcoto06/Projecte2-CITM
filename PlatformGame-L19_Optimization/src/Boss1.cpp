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

	R_Hand = new Hand();
	R_Hand->attacking = false;
	R_Hand->position = initialHeadPos + Vector2D(-400, 1000);
	R_Hand->pbody = Engine::GetInstance().physics->CreateRectangleSensor(R_Hand->idlePos.getX(), R_Hand->idlePos.getY(), 200, 300, bodyType::STATIC);
	R_Hand->pbody->ctype = ColliderType::BOSS_R_HAND;
	R_Hand->pbody->listener = this;

	L_Hand = new Hand();
	L_Hand->attacking = false;
	L_Hand->h_speed *= -1;
	L_Hand->position = initialHeadPos + Vector2D(400, 1000);
	L_Hand->pbody = Engine::GetInstance().physics->CreateRectangleSensor(L_Hand->idlePos.getX(), L_Hand->idlePos.getY(), 200, 300, bodyType::STATIC);
	L_Hand->pbody->ctype = ColliderType::BOSS_R_HAND;
	L_Hand->pbody->listener = this;

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
	animations.push_back(&idle_body->animation);

	intro_body = new bossAnimation(60, "intro", Body_Parts::BODY, false);
	for (int i = 1; i <= intro_body->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/INTRO/INTRO_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		intro_body->animation.push_back(frame);
	}
	animations.push_back(&intro_body->animation);

	hurt_body = new bossAnimation(16, "hurt", Body_Parts::BODY, false);
	for (int i = 1; i <= hurt_body->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/B.HURT/B.HURT_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		hurt_body->animation.push_back(frame);
	}
	animations.push_back(&hurt_body->animation);

	stun_body = new bossAnimation(28, "stun", Body_Parts::BODY, false);
	for (int i = 1; i <= stun_body->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/STUN/STUN_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		stun_body->animation.push_back(frame);
	}
	animations.push_back(&stun_body->animation);

	suck_body = new bossAnimation(5, "suck", Body_Parts::BODY, true);
	for (int i = 1; i <= suck_body->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/SUCK/SUCK_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		suck_body->animation.push_back(frame);
	}
	animations.push_back(&suck_body->animation);

	death_body = new bossAnimation(150, "death", Body_Parts::BODY, false);
	for (int i = 1; i <= death_body->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/DEATH/DEATH_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		death_body->animation.push_back(frame);
	}
	animations.push_back(&death_body->animation);

	//HAND ANIMATIONS

	intro_R_hand = new bossAnimation(1, "intro", Body_Parts::R_HAND, true);
	{
		SDL_Texture* frame = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Bosses/Boss1/RightHand/IDLE/IDLE_SS_1.png");
		intro_R_hand->animation.push_back(frame);
	}
	animations.push_back(&intro_R_hand->animation);

	idle_R_hand = new bossAnimation(18, "idle", Body_Parts::R_HAND, true);
	for (int i = 1; i <= idle_R_hand->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/RightHand/IDLE/IDLE_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		idle_R_hand->animation.push_back(frame);
	}
	animations.push_back(&idle_R_hand->animation);

	vertical_R_hand = new bossAnimation(18, "vertical", Body_Parts::R_HAND, false);
	for (int i = 1; i <= vertical_R_hand->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/RightHand/VerticalATA/VerticalATA_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		vertical_R_hand->animation.push_back(frame);
	}
	animations.push_back(&vertical_R_hand->animation);

	horizontal_R_hand = new bossAnimation(18, "horizontal", Body_Parts::R_HAND, false);
	for (int i = 1; i <= horizontal_R_hand->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/RightHand/HorizontalATA/HorizontalATA_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		horizontal_R_hand->animation.push_back(frame);
	}
	animations.push_back(&horizontal_R_hand->animation);

	hurt_R_hand = new bossAnimation(12, "hurt", Body_Parts::R_HAND, false);
	for (int i = 1; i <= hurt_R_hand->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/RightHand/Hurt/Hurt_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		hurt_R_hand->animation.push_back(frame);
	}
	animations.push_back(&hurt_R_hand->animation);

	intro_L_hand = new bossAnimation(1, "intro", Body_Parts::L_HAND, true);
	{
		SDL_Texture* frame = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Bosses/Boss1/LeftHand/IDLE/IDLE_SS_1.png");
		intro_L_hand->animation.push_back(frame);
	}
	animations.push_back(&intro_L_hand->animation);

	idle_L_hand = new bossAnimation(18, "idle", Body_Parts::L_HAND, true);
	for (int i = 1; i <= idle_L_hand->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/LeftHand/IDLE/IDLE_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		idle_L_hand->animation.push_back(frame);
	}
	animations.push_back(&idle_L_hand->animation);

	vertical_L_hand = new bossAnimation(18, "vertical", Body_Parts::L_HAND, false);
	for (int i = 1; i <= vertical_L_hand->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/LeftHand/VerticalATA/VerticalATA_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		vertical_L_hand->animation.push_back(frame);
	}
	animations.push_back(&vertical_L_hand->animation);

	horizontal_L_hand = new bossAnimation(18, "horizontal", Body_Parts::L_HAND, false);
	for (int i = 1; i <= horizontal_L_hand->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/LeftHand/HorizontalATA/HorizontalATA_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		horizontal_L_hand->animation.push_back(frame);
	}
	animations.push_back(&horizontal_L_hand->animation);

	hurt_L_hand = new bossAnimation(12, "hurt", Body_Parts::L_HAND, false);
	for (int i = 1; i <= hurt_L_hand->frames; ++i)
	{
		std::string frameStr = std::to_string(i);
		std::string pathStr = "Assets/Textures/Characters/Bosses/Boss1/LeftHand/Hurt/Hurt_SS_" + frameStr + ".png";
		const char* path = pathStr.c_str();
		SDL_Texture* frame = Engine::GetInstance().textures->Load(path);
		hurt_L_hand->animation.push_back(frame);
	}
	animations.push_back(&hurt_L_hand->animation);

	return true;
}

bool Boss1::Update(float dt)
{
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_B) == KEY_DOWN && !active) {
		Initialize();
	}
	if (!active) return true;
	ZoneScoped;

	GetPhysicsValues();

	//Func_EnemyStates(dt);
	ApplyPhysics();
	//RIGHT HAND
	if ((R_Hand->velocity.y == -R_Hand->v_speed && R_Hand->position.getY() < R_Hand->idlePos.getY()) || (R_Hand->velocity.x == -R_Hand->h_speed && R_Hand->position.getX() < R_Hand->idlePos.getX()))
	{
		R_Hand->velocity = b2Vec2_zero;
		R_Hand->attackCooldown = SDL_rand(5);
		R_Hand->position = Vector2D(R_Hand->idlePos.getX(), R_Hand->idlePos.getY());
		R_Hand->attackTimer.Start();
		R_Hand->attacking = false;
	}
	if (R_Hand->attackTimer.ReadSec() >= R_Hand->attackCooldown && currentBodyAnimation->name == "idle" && currentRHandAnimation->name == "idle" && R_Hand->attacking == false) {
		Attack(R_Hand);
	}
	//LEFT HAND
	if ((L_Hand->velocity.y == -L_Hand->v_speed && L_Hand->position.getY() < L_Hand->idlePos.getY()) || (L_Hand->velocity.x == -L_Hand->h_speed && L_Hand->position.getX() > L_Hand->idlePos.getX()))
	{
		L_Hand->velocity = b2Vec2_zero;
		L_Hand->attackCooldown = SDL_rand(5);
		L_Hand->position = Vector2D(L_Hand->idlePos.getX(), L_Hand->idlePos.getY());
		L_Hand->attackTimer.Start();
		L_Hand->attacking = false;
	}
	if (L_Hand->attackTimer.ReadSec() >= L_Hand->attackCooldown && currentBodyAnimation->name == "idle" && currentLHandAnimation->name == "idle" && L_Hand->attacking == false) {
		Attack(L_Hand);
	}
	MoveHands(dt);
	Draw(dt);

	return true;
}

void Boss1::GetPhysicsValues() {
	return;
}

void Boss1::ApplyPhysics() {

	//Apply velocity via helper
	return;
}

void Boss1::MoveHands(float dt) {
	//Move Hands
	R_Hand->position.setX(R_Hand->position.getX() + (R_Hand->velocity.x * dt));
	R_Hand->position.setY(R_Hand->position.getY() + (R_Hand->velocity.y * dt));
	R_Hand->pbody->SetPosition(R_Hand->position.getX(), R_Hand->position.getY());

	L_Hand->position.setX(L_Hand->position.getX() + (L_Hand->velocity.x * dt));
	L_Hand->position.setY(L_Hand->position.getY() + (L_Hand->velocity.y * dt));
	L_Hand->pbody->SetPosition(L_Hand->position.getX(), L_Hand->position.getY());
	return;
}

void Boss1::Draw(float dt)
{
	//Draw Body
	totalBodyAnimFrames = currentBodyAnimation->frames;
	totalRHandAnimFrames = currentRHandAnimation->frames;
	totalLHandAnimFrames = currentLHandAnimation->frames;
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
		//RIGHT HAND
		if (currentRHandFrame < totalRHandAnimFrames) {
			currentRHandFrame += 1;
			if (currentRHandFrame == 13 && currentRHandAnimation == horizontal_R_hand) R_Hand->velocity.x = R_Hand->h_speed;
		}
		else if (currentRHandAnimation->loop == true) {
			currentRHandFrame = 1;
		}
		else {
			AnimationFinished(currentRHandAnimation);
		}
		//LEFT HAND
		if (currentLHandFrame < totalLHandAnimFrames) {
			currentLHandFrame += 1;
			if (currentLHandFrame == 13 && currentLHandAnimation == horizontal_L_hand) L_Hand->velocity.x = L_Hand->h_speed;
		}
		else if (currentLHandAnimation->loop == true) {
			currentLHandFrame = 1;
		}
		else {
			AnimationFinished(currentLHandAnimation);
		}
	}
	Engine::GetInstance().render->DrawTexture(currentBodyAnimation->animation.at(currentBodyFrame - 1), position.getX(), position.getY(), NULL);
	Engine::GetInstance().render->DrawTexture(currentRHandAnimation->animation.at(currentRHandFrame - 1), R_Hand->position.getX() - 512, R_Hand->position.getY() - 600, NULL);
	Engine::GetInstance().render->DrawTexture(currentLHandAnimation->animation.at(currentLHandFrame - 1), L_Hand->position.getX() - 512, L_Hand->position.getY() - 600, NULL);
}

bool Boss1::CleanUp()
{
	LOG("Cleanup enemy");
	for (int i = 0; i < animations.size(); ++i) {
		for (int j = 0; j < animations.at(i)->size(); ++j) {
			Engine::GetInstance().textures->UnLoad(animations.at(i)->at(j));
		}
	}
	Engine::GetInstance().physics->DeletePhysBody(head_body);
	Engine::GetInstance().physics->DeletePhysBody(R_Hand->pbody);
	Engine::GetInstance().physics->DeletePhysBody(L_Hand->pbody);
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
	/*if (player->playerCurrentHp < player->playerMaxHp) {
		player->playerCurrentHp += 1;
	}
	player->healing = true;
	player->effectAnims.SetCurrent("lifeUp");*/
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
			case ColliderType::BOSS_R_HAND:
				PlayAnimation(hurt_R_hand);
				break;
			case ColliderType::BOSS_L_HAND:
				PlayAnimation(hurt_L_hand);
				break;
			}
		}
		break;
	case ColliderType::SUCK_ZONE:
		if (currentBodyAnimation == suck_body) {
			PlayAnimation(death_body);
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
	//BODY ANIMATIONS
	if (animation->part == Body_Parts::BODY) {
		if (animation->name == "intro") {
			head_body = Engine::GetInstance().physics->CreateRectangleSensor(0, 0, 200, 300, bodyType::STATIC);
			SetPosition(initialHeadPos);
			head_body->ctype = ColliderType::BOSS_HEAD;
			head_body->listener = this;
			PlayAnimation(idle_body);
			//RIGHT HAND
			R_Hand->velocity.y = 0;
			int Rx, Ry;
			R_Hand->pbody->GetPosition(Rx, Ry);
			R_Hand->idlePos = Vector2D(Rx, Ry);
			PlayAnimation(idle_R_hand);
			R_Hand->attackCooldown = SDL_rand(5);
			R_Hand->attackTimer.Start();
			//LEFT HAND
			L_Hand->velocity.y = 0;
			int Lx, Ly;
			L_Hand->pbody->GetPosition(Lx, Ly);
			L_Hand->idlePos = Vector2D(Lx, Ly);
			PlayAnimation(idle_L_hand);
			L_Hand->attackCooldown = SDL_rand(5);
			L_Hand->attackTimer.Start();
		}
		else if (animation->name == "hurt") {
			if (life > 0) {
				switch (animation->part) {
				case Body_Parts::BODY:
					PlayAnimation(idle_body);
					break;
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
			PlayAnimation(suck_body);
		}
		else if (animation->name == "death") {
			Destroy();
		}
	}
	
	//HAND ANIMATIONS
	if (animation->part == Body_Parts::R_HAND || animation->part == Body_Parts::L_HAND) {
		if ((animation->name == "vertical" || animation->name == "horizontal" || animation->name == "hurt"))
		{
			if (animation->part == Body_Parts::R_HAND) {
				if (animation->name == "horizontal" || (int)R_Hand->position.getX() > (int)R_Hand->idlePos.getX()) {
					R_Hand->velocity.x = -R_Hand->h_speed;
				}
				if (animation->name == "vertical" || (int)R_Hand->position.getY() > (int)R_Hand->idlePos.getY()) {
					R_Hand->velocity.y = -R_Hand->v_speed;
				}
				if (life > 0) {
					PlayAnimation(idle_R_hand);
				}
			}
			else {
				if (animation->name == "horizontal" || (int)L_Hand->position.getX() > (int)L_Hand->idlePos.getX()) {
					L_Hand->velocity.x = -L_Hand->h_speed;
				}
				if (animation->name == "vertical" || (int)L_Hand->position.getY() > (int)L_Hand->idlePos.getY()) {
					L_Hand->velocity.y = -R_Hand->v_speed;
				}
				if (life > 0) {
					PlayAnimation(idle_L_hand);
				}
			}
			if (life <= 0) {
				Engine::GetInstance().physics->DeletePhysBody(head_body);
				PlayAnimation(stun_body);
				PlayAnimation(intro_R_hand);
				PlayAnimation(intro_L_hand);
				R_Hand->velocity.y = 0.5;
				L_Hand->velocity.y = 0.5;
			}
		}
	}
	return;
}

void Boss1::PlayAnimation(bossAnimation* animation)
{
	if (animation->part == Body_Parts::BODY)
	{
		currentBodyFrame = 1;
		currentBodyAnimation = animation;
	}else if (animation->part == Body_Parts::R_HAND)
	{
		currentRHandFrame = 1;
		currentRHandAnimation = animation;
	}
	else if (animation->part == Body_Parts::L_HAND)
	{
		currentLHandFrame = 1;
		currentLHandAnimation = animation;
	}
}

Boss1::bossAnimation::bossAnimation(int frames, std::string name, Body_Parts part, bool loop) {
	this->frames = frames;
	this->name = name;
	this->part = part;
	this->loop = loop;
}

void Boss1::Initialize() {
	//Set Current Animation as intro
	currentBodyAnimation = intro_body;
	currentRHandAnimation = intro_R_hand;
	currentLHandAnimation = intro_L_hand;
	frameTimer.Start();
	R_Hand->velocity.y = -0.15f;
	L_Hand->velocity.y = -0.15f;
	active = true;
}

void Boss1::Attack(Hand* hand) {
	int attack = SDL_rand(2);
	if (hand == R_Hand) {
		R_Hand->attacking = true;
		if (attack == 1)
		{
			PlayAnimation(vertical_R_hand);
			R_Hand->velocity.y = R_Hand->v_speed;
		}
		else {
			PlayAnimation(horizontal_R_hand);
		}
	}
	else {
		L_Hand->attacking = true;
		if (attack == 1)
		{
			PlayAnimation(vertical_L_hand);
			L_Hand->velocity.y = L_Hand->v_speed;
		}
		else {
			PlayAnimation(horizontal_L_hand);
		}
	}
}