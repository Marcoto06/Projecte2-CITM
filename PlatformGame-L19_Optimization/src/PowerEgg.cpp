#include "PowerEgg.h"
#include "Player.h"
#include "Engine.h"
#include "Log.h"

PowerEgg::PowerEgg() : Entity(EntityType::POWER_EGG)
{
	name = "Power Egg";
}

PowerEgg::~PowerEgg() {

}

bool PowerEgg::Awake() {
	return true;
}

bool PowerEgg::Start() {
	//Carregar textura i animacions
	pbody = Engine::GetInstance().physics->CreateRectangleSensor(position.getX(), position.getY(), w, h, bodyType::STATIC);
	pbody->ctype = ColliderType::POWER_EGG;
	pbody->listener = this;
	return true;
}

bool PowerEgg::Update(float dt)
{
	if (texture != nullptr)
	{
		anims.Update(dt);
		SDL_Rect rect = anims.GetCurrentFrame();

		if (rect.w <= 0 || rect.h <= 0) {
			rect = { 0, 0, 32, 32 };
		}

		Engine::GetInstance().render->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &rect);
	}
	else
	{
		LOG("ERROR FATAL: La textura del coleccionable es NULL.");
	}

	if (picked)
	{
		Destroy();
	}
	return true;
}

bool PowerEgg::CleanUp()
{
	if (texture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(texture);
		texture = nullptr;
	}

	if (pbody != nullptr)
	{
		Engine::GetInstance().physics->DeletePhysBody(pbody);
		pbody = nullptr;
	}

	return true;
}

bool PowerEgg::Destroy()
{
	active = false;

	if (this->tiledId != -1) {
		Engine::GetInstance().scene->destroyedEntitiesIds.push_back(this->tiledId);
	}

	pendingToDelete = true;
	return true;
}

void PowerEgg::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (picked) return;

	PhysBody* other = (physA == pbody) ? physB : physA;

	if (other->ctype == ColliderType::PLAYER)
	{
		Player* player = (Player*)other->listener;

		if (player != nullptr)
		{
			picked = true;
		}
	}
}


