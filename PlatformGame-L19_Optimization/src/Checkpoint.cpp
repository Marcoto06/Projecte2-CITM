#include "Checkpoint.h"
#include "Engine.h"
#include "Physics.h"
#include "Player.h"
#include "Scene.h"
#include "Textures.h"
#include "Render.h"
#include "Log.h"
#include <algorithm>

std::vector<Checkpoint*> Checkpoint::allCheckpoints;

Checkpoint::Checkpoint() :Entity(EntityType::CHECKPOINT) 
{
	name = "Checkpoint";
}

Checkpoint::~Checkpoint(){}

bool Checkpoint::Start() 
{
    allCheckpoints.push_back(this);

    texture = Engine::GetInstance().textures->Load("Assets/Textures/Entities/Atlas_Punto_Guardado.png");

    std::unordered_map<int, std::string> aliases;
    aliases[0] = "inactive";
    aliases[1] = "active";
    aliases[24] = "deactivate";

    anims.LoadFromTSX("Assets/Textures/Entities/Atlas_Punto_Guardado.tsx", aliases); 
    anims.Func_SetAnimationLoop("active", false);
    anims.Func_SetAnimationLoop("deactivate", false);

    if (isActive)
    {
        anims.SetCurrent("active");
    }
    else
    {
        anims.SetCurrent("inactive");
    }    

    int startX = (int)position.getX();
    int startY = (int)position.getY();

    int centerX = startX + (w / 2) + 90;
	int centerY = startY + (h / 2) + 150;

    pbody = Engine::GetInstance().physics->CreateRectangleSensor(centerX, centerY, 96, 128, bodyType::STATIC);
    pbody->ctype = ColliderType::CHECKPOINT;
    pbody->listener = this;

    return true;
}

bool Checkpoint::Update(float dt) 
{
    anims.Update(dt);

    if (anims.GetCurrentName() == "deactivate"  && anims.HasCurrentAnimationFinished())
    {
        anims.SetCurrent("inactive");
    }

    if (texture != nullptr)
    {
        const SDL_Rect& rect = anims.GetCurrentFrame();
        Engine::GetInstance().render->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &rect);
    }

    return true;
}

bool Checkpoint::CleanUp() 
{
    if (!allCheckpoints.empty())
    {
        auto it = std::remove(allCheckpoints.begin(), allCheckpoints.end(), this);
        if (it != allCheckpoints.end()) {
            allCheckpoints.erase(it, allCheckpoints.end());
        }
    }

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

bool Checkpoint::Destroy()
{
    LOG("Destroying Checkpoint");
    active = false;

    if (this->tiledId != -1) {
        Engine::GetInstance().scene->destroyedEntitiesIds.push_back(this->tiledId);
    }

    pendingToDelete = true;
    return true;
}

void Checkpoint::OnCollision(PhysBody* physA, PhysBody* physB) 
{
    if (isActive) return;

    PhysBody* other = (physA == pbody) ? physB : physA;

    if (other->ctype == ColliderType::PLAYER) 
    {
        Player* player = (Player*)other->listener;

        if (player != nullptr) 
        {
            player->SetRespawnPosition(position);
            LOG("Checkpoint activado en: %f, %f", position.getX(), position.getY());

            for (Checkpoint* cp : allCheckpoints)
            {
                if (cp != this && cp->IsActive())
                {
                    cp->SetActive(false);
                }
            }
            player->playerCurrentHp = player->playerMaxHp;

            SetActive(true);

			Engine::GetInstance().scene->SaveGame();
        }
    }
}

void Checkpoint::SetActive(bool active) {

    if (active) {
        isActive = true;
        anims.SetCurrent("active");
    }
    else {

        if (isActive)
        {
            isActive = false;
            anims.SetCurrent("deactivate");
        }
        else
        {
            anims.SetCurrent("inactive");
        }
    }
}