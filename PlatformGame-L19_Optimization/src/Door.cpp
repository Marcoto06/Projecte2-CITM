#include "Door.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"

Door::Door() : Entity(EntityType::DOOR)
{
	name = "door";
}

Door::~Door() {}

bool Door::Awake() {
	return true;
}

bool Door::Start() {

	pbody = Engine::GetInstance().physics->CreateRectangle(position.getX(), position.getY(), width, height, bodyType::STATIC);

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::DOOR;

	// Set this class as the listener of the pbody
	pbody->listener = this;   // so Begin/EndContact can call back to Item

	return true;
}

bool Door::Update(float dt)
{
	return true;
}

bool Door::CleanUp()
{
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

bool Door::Destroy()
{
	LOG("Destroying door");
	pendingToDelete = true;
	return true;
}

void Door::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		Engine::GetInstance().scene->UnloadLevel();
		Engine::GetInstance().scene->LoadLevel(destination);
	}
}