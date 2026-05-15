#include "Climbable.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"

Climbable::Climbable() : Entity(EntityType::CLIMBABLE)
{
	name = "climbable";
}

Climbable::~Climbable() {}

bool Climbable::Awake() {
	return true;
}

bool Climbable::Start() {

	pbody = Engine::GetInstance().physics->CreateRectangleSensor(position.getX(), position.getY(), width, height, bodyType::STATIC);

	climbPoint = position.getX() - width/2 + 10;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::CLIMBABLE;

	// Set this class as the listener of the pbody
	pbody->listener = this;   // so Begin/EndContact can call back to Item

	return true;
}

bool Climbable::Update(float dt)
{
	return true;
}

bool Climbable::CleanUp()
{
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

bool Climbable::Destroy()
{
	LOG("Destroying climbable");
	pendingToDelete = true;
	return true;
}

void Climbable::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		Engine::GetInstance().scene->player->nearestClimbable = this;
	}
}