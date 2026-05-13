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
	std::unordered_map<int, std::string> aliases;
	aliases = { {0, "idle"}, {11, "die"} };
	anims.LoadFromTSX("Assets/Textures/Entities/Atlas_Huevo.tsx", aliases);
	texture = Engine::GetInstance().textures->Load("Assets/Textures/Entities/Atlas_Huevo.png");
	anims.SetCurrent("idle");
	anims.Func_SetAnimationLoop("die", false);

	pbody = Engine::GetInstance().physics->CreateRectangleSensor(position.getX() + w / 2, position.getY() + h / 2, w, h, bodyType::STATIC);
	pbody->ctype = ColliderType::POWER_EGG;
	pbody->listener = this;

	player = Engine::GetInstance().scene->player;
	return true;
}

bool PowerEgg::Update(float dt)
{
	if (texture != nullptr)
	{
		anims.Update(dt);
		SDL_Rect rect = anims.GetCurrentFrame();

		if (rect.w <= 0 || rect.h <= 0) {
			rect = { 0, 0, 128, 128 };
		}

		Engine::GetInstance().render->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &rect);
	}
	else
	{
		LOG("ERROR FATAL: La textura del coleccionable es NULL.");
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
	PhysBody* other = (physA == pbody) ? physB : physA;

	if (other->ctype == ColliderType::SUCK_ZONE)
	{
		picked = true;
		anims.SetCurrent("die");
		switch (assimilate) {
		case 1:
			player->hasWallJump = true;
			break;
		case 2:
			player->hasCrouch = true;
			break;
		case 3:
			break;
		case 4:
			break;
		}
	}
}


