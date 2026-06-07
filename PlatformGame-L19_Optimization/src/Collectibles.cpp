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
	case 1:
		//Saliva
		aliases = { {0, "idle"} };
		anims.LoadFromTSX("Assets/Textures/Collectibles/Saliva_Coleccionable.tsx", aliases);
		texture = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Saliva_Coleccionable.png");
		anims.SetCurrent("idle");
	case 2:
		//Tejido pulmonar inflamado
		aliases = { {0, "idle"} };
		anims.LoadFromTSX("Assets/Textures/Collectibles/TejidoAlveolar_Coleccionable.tsx", aliases);
		texture = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/TejidoAlveolar_Coleccionable.png");
		anims.SetCurrent("idle");
		break;
	case 3:
		//Cilios pulmonares
		aliases = { {0, "idle"} };
		anims.LoadFromTSX("Assets/Textures/Collectibles/Cilios_Coleccionables.tsx", aliases);
		texture = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Cilios_Coleccionables.png");
		anims.SetCurrent("idle");
		break;
	case 4:
		//Extracto de mucosa pulmonar solidificada
		aliases = { {0, "idle"} };
		anims.LoadFromTSX("Assets/Textures/Collectibles/ExtractoMucosaPulmonar_Coleccionable.tsx", aliases);
		texture = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/ExtractoMucosaPulmonar_Coleccionable.png");
		anims.SetCurrent("idle");
		break;
	case 5:
		//Neocito Extracto
		aliases = { {0, "idle"} };
		anims.LoadFromTSX("Assets/Textures/Collectibles/Neumocito_Coleccionable.tsx", aliases);
		texture = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Neumocito_Coleccionable.png");
		anims.SetCurrent("idle");
		break;
	case 6:
		//Tejido necrotizado funcional
		aliases = { {0, "idle"} };
		anims.LoadFromTSX("Assets/Textures/Collectibles/Tejido_Necrotizado_Coleccionable.tsx", aliases);
		texture = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Tejido_Necrotizado_Coleccionable.png");
		anims.SetCurrent("idle");
		break;
	case 7:
		//Pétalo/Rama de tejidos humanos
		aliases = { {0, "idle"} };
		anims.LoadFromTSX("Assets/Textures/Collectibles/Rama_Coleccionable.tsx", aliases);
		texture = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Rama_Coleccionable.png");
		anims.SetCurrent("idle");
		break;
	case 13:
		//Extracto de células parietales
		aliases = { {0, "idle"} };
		anims.LoadFromTSX("Assets/Textures/Collectibles/ExtractoCelulaParietal_Coleccionable.tsx", aliases);
		texture = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/ExtractoCelulaParietal_Coleccionable.png");
		anims.SetCurrent("idle");
		break;
	case 14:
		//Extracto de células epiteliales
		aliases = { {0, "idle"} };
		anims.LoadFromTSX("Assets/Textures/Collectibles/Extracto_celula_Coleccionable_epitelial.tsx", aliases);
		texture = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Extracto_celula_Coleccionable_epitelial.png");
		anims.SetCurrent("idle");
		break;
	case 15:
		//Tejido estomacal
		aliases = { {0, "idle"} };
		anims.LoadFromTSX("Assets/Textures/Collectibles/TejidoEstomacal_Coleccionable.tsx", aliases);
		texture = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/TejidoEstomacal_Coleccionable.png");
		anims.SetCurrent("idle");
		break;
	case 16:
		//Mucosa gástrica
		aliases = { {0, "idle"} };
		anims.LoadFromTSX("Assets/Textures/Collectibles/MucosaGastrica_Coleccioable.tsx", aliases);
		texture = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/MucosaGastrica_Coleccioable.png");
		anims.SetCurrent("idle");
		break;
	case 17:
		//Formaciónes ulcerosas
		aliases = { {0, "idle"} };
		anims.LoadFromTSX("Assets/Textures/Collectibles/Formacion_Ulcerosa_Coleccionable.tsx", aliases);
		texture = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Formacion_Ulcerosa_Coleccionable.png");
		anims.SetCurrent("idle");
		break;
	case 18:
		//Masa carcomida de tejido
		aliases = { {0, "idle"} };
		anims.LoadFromTSX("Assets/Textures/Collectibles/Masa_Carcomida_Coleccionable.tsx", aliases);
		texture = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Masa_Carcomida_Coleccionable.png");
		anims.SetCurrent("idle");
		break;
	case 19:
		//Extractos de crisalida Solitaria
		aliases = { {0, "idle"} };
		anims.LoadFromTSX("Assets/Textures/Collectibles/ExtractoCrisalida_Coleccionable.tsx", aliases);
		texture = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/ExtractoCrisalida_Coleccionable.png");
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

			if (c_num == 1)
			{
				Engine::GetInstance().scene->TriggerFallingVideo(destMap, destX, destY);
			}
		}
	}
}


