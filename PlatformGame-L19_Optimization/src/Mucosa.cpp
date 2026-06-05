#include "Mucosa.h"
#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Log.h"
#include "Physics.h"
#include <cmath>

Mucosa::Mucosa() : Entity(EntityType::MUCOSA)
{
	name = "Mucosa";
}

Mucosa::~Mucosa()
{
}

bool Mucosa::Awake()
{
	return true;
}

bool Mucosa::Start()
{
	std::unordered_map<int, std::string> aliases = {{0, "projectile"}};

	projectileAnims.LoadFromTSX("Assets/Textures/Characters/Bosses/Boss2/Mucosa_SS.tsx", aliases);
	projectileAnims.SetCurrent("projectile");
	projectileAnims.Func_SetAnimationLoop("projectile", true);

	projectileTexture = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Bosses/Boss2/Mucosa_SS.png");

	texW = 256;
	texH = 256;

	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(),(int)position.getY(), 20, bodyType::DYNAMIC);

	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;

	return true;
}

bool Mucosa::Update(float dt)
{
	if (state == 0) projectileAnims.Update(dt);
	if (state == 2) electricAnims.Update(dt);

	ApplyPhysics();
	Draw(dt);
	return true;
}

void Mucosa::ApplyPhysics()
{
	if (pbody == nullptr)
	{
		return;
	}

	velocity.y = Engine::GetInstance().physics->GetYVelocity(pbody);

	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void Mucosa::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	default:
		break;
	}
}

void Mucosa::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
}

void Mucosa::Draw(float dt)
{
	if (state == 0)
	{
		const SDL_Rect& animFrame = projectileAnims.GetCurrentFrame();

		int frameW = animFrame.w;
		int frameH = animFrame.h;

		int drawX = (int)position.getX() - (frameW / 2);
		int drawY = (int)position.getY() - (frameH / 2);
		Engine::GetInstance().render->DrawTexture(projectileTexture, drawX, drawY, &animFrame, 1.0f, 0.0, frameW / 2, frameH / 2, SDL_FLIP_NONE, 1.0f);
	}
	else 
	{
		int drawX, drawY;
		pbody->GetPosition(drawX, drawY);
		Engine::GetInstance().render->DrawTexture(staticTexture, drawX, drawY, NULL);
	}
}

bool Mucosa::CleanUp()
{
	if (projectileTexture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(projectileTexture);
		projectileTexture = nullptr;
	}
	if (staticTexture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(staticTexture);
		staticTexture = nullptr;
	}
	if (electricTexture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(electricTexture);
		electricTexture = nullptr;
	}

	if (pbody != nullptr)
	{
		Engine::GetInstance().physics->DeletePhysBody(pbody);
		pbody = nullptr;
	}

	return true;
}

bool Mucosa::Destroy()
{
	active = false;
	pendingToDelete = true;
	return true;
}