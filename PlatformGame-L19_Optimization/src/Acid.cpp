#include "Acid.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"

Acid::Acid() : Entity(EntityType::ACID)
{
	name = "acid";
}

Acid::~Acid() {}

bool Acid::Awake() {
	return true;
}

bool Acid::Start() {

	pbody = Engine::GetInstance().physics->CreateRectangle(position.getX(), position.getY(), width, height, bodyType::STATIC);

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::ACID;

	// Set this class as the listener of the pbody
	pbody->listener = this;   // so Begin/EndContact can call back to Item

	return true;
}

bool Acid::Update(float dt)
{
	return true;
}

bool Acid::CleanUp()
{
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

bool Acid::Destroy()
{
	LOG("Destroying acid");
	pendingToDelete = true;
	return true;
}