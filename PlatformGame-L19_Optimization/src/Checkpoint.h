#pragma once
#include "Entity.h"
#include "Physics.h"
#include "Animation.h"
#include <vector>

struct SDL_Texture;

class Checkpoint : public Entity {
public:
    Checkpoint();
    virtual ~Checkpoint();

    bool Start();
    bool Update(float dt);
    bool CleanUp();
    bool Destroy() override;

    void OnCollision(PhysBody* physA, PhysBody* physB) override;

    void SetActive(bool active);

public:
    static std::vector<Checkpoint*> allCheckpoints;

    bool IsActive() const { return isActive; }

private:
    PhysBody* pbody;
    
    SDL_Texture* texture = nullptr;
    AnimationSet anims;

    int w = 64, h = 64;

    bool isActive = false;
};