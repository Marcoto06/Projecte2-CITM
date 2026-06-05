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
	std::unordered_map<int, std::string> aliases = {{0, "loop"}};

	projectileAnims.LoadFromTSX("Assets/Textures/Characters/Bosses/Boss2/Mucosa_SS.tsx", aliases);
	projectileAnims.SetCurrent("loop");
	projectileAnims.Func_SetAnimationLoop("loop", true);

	projectileTexture = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Bosses/Boss2/Mucosa_SS.png");

	electricAnims.LoadFromTSX("Assets/Textures/Characters/Bosses/Boss2/Rayos_SS.tsx", aliases);
	electricAnims.SetCurrent("loop");
	electricAnims.Func_SetAnimationLoop("loop", true);

	electricTexture = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Bosses/Boss2/Rayos_SS.png");

	staticTexture = Engine::GetInstance().textures->Load("Assets/Textures/Characters/Bosses/Boss2/Moco_SS.png");

	texW = 256;
	texH = 256;

	state = 0;

	return true;
}

bool Mucosa::Update(float dt)
{
	if (state == 0) return true;
	if (state == 1) 
	{
		projectileAnims.Update(dt);
		ApplyPhysics();
	}
	if (state == 3) electricAnims.Update(dt);

	Draw(dt);
	return true;
}

void Mucosa::ApplyPhysics()
{
	if (pbody == nullptr)
	{
		return;
	}

	velocity.x = 0;
	velocity.y = Engine::GetInstance().physics->GetYVelocity(pbody);

	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void Mucosa::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		state = 2;
	default:
		break;
	}
}

void Mucosa::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
}

void Mucosa::Draw(float dt)
{
	if (state == 1)
	{
		const SDL_Rect& animFrame = projectileAnims.GetCurrentFrame();

		int frameW = animFrame.w;
		int frameH = animFrame.h;
		
		int drawX, drawY;
		pbody->GetPosition(drawX, drawY);
		drawX -= 256 / 2;
		drawY -= 256 / 2;
		Engine::GetInstance().render->DrawTexture(projectileTexture, drawX, drawY, &animFrame, 1.0f, 0.0, frameW / 2, frameH / 2, SDL_FLIP_NONE, 1.0f);
	}
	else if (state > 1)
	{
		int drawX, drawY;
		pbody->GetPosition(drawX, drawY);
		drawX -= 256 / 2;
		drawY -= 256;
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

void Mucosa::Spawn() 
{
	position = spawnPos;
	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(), (int)position.getY(), 20, bodyType::DYNAMIC);

	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;

	state = 1;
}