#include "Collectibles.h"
#include "Player.h"
#include "Engine.h"
#include "Log.h"


std::vector<Collectibles*> allCollectibles;

Collectibles::Collectibles() : Entity(EntityType::COLLECTIBLES)
{
	name = "Collectibles";
}

Collectibles::~Collectibles() {

}

bool Collectibles::Awake() {
	return true;
}

bool Collectibles::Start() {

	allCollectibles.push_back(this);
	std::unordered_map<int, std::string> aliases;

	switch (c_num)
	{
	case 0:
		//Amigdala
		aliases = { {0, "idle"} };
		anims.LoadFromTSX("Assets/Textures/Collectibles/Amigdala_Coleccionable.tsx", aliases);
		texture = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Amigdala_Coleccionable.png");
		anims.SetCurrent("idle");
		break;
	default:
		break;
	}
	
	w = 32;
	h = 32;

	int startX = (int)position.getX();
	int startY = (int)position.getY();

	int centerX = startX + (w / 2);
	int centerY = startY + (h / 2);

	pbody = Engine::GetInstance().physics->CreateRectangleSensor(centerX, centerY, w, h, bodyType::STATIC);
	pbody->ctype = ColliderType::COLLECTIBLES; 
	pbody->listener = this;
	return true;
}

bool Collectibles::Update(float dt)
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

bool Collectibles::CleanUp()
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

bool Collectibles::Destroy()
{
	active = false;

	if (this->tiledId != -1) {
		Engine::GetInstance().scene->destroyedEntitiesIds.push_back(this->tiledId);
	}

	pendingToDelete = true;
	return true;
}

void Collectibles::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (picked) return;

	PhysBody* other = (physA == pbody) ? physB : physA;

	if (other->ctype == ColliderType::PLAYER)
	{
		Player* player = (Player*)other->listener;

		if (player != nullptr)
		{
			player->list_collectibles.push_back(this->c_num);
			picked = true;
		}
	}
}


