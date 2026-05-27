#include "Electric.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"

Electric::Electric() : Entity(EntityType::ELECTRIC)
{
	name = "electric";
}

Electric::~Electric() {}

bool Electric::Awake() {
	return true;
}

bool Electric::Start() {

	pbody = Engine::GetInstance().physics->CreateRectangleSensor(position.getX(), position.getY(), width, height, bodyType::STATIC);

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::UNKNOWN;

	// Set this class as the listener of the pbody
	pbody->listener = this;   // so Begin/EndContact can call back to Item

	stateTimer.Start();

	return true;
}

bool Electric::Update(float dt)
{
	if (stateTimer.ReadSec() >= timeBetween && pbody->ctype == ColliderType::UNKNOWN)
	{
		pbody->ctype = ColliderType::ENEMY;
		stateTimer.Start();
	}
	if(stateTimer.ReadSec() >= electricTime && pbody->ctype == ColliderType::ENEMY)
	{
		pbody->ctype = ColliderType::UNKNOWN;
		stateTimer.Start();
	}
	return true;
}

bool Electric::CleanUp()
{
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

bool Electric::Destroy()
{
	LOG("Destroying electric");
	pendingToDelete = true;
	return true;
}