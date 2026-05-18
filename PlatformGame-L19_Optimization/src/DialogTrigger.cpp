#include "DialogTrigger.h"
#include "Player.h"
#include "Engine.h"
#include "Log.h"
#include "DialogManager.h"

DialogTrigger::DialogTrigger() : Entity(EntityType::DIALOG_TRIGGER)
{
	name = "Dialog Trigger";
}

DialogTrigger::~DialogTrigger() {

}

bool DialogTrigger::Awake() {
	return true;
}

bool DialogTrigger::Start() {

	pbody = Engine::GetInstance().physics->CreateRectangleSensor(position.getX(), position.getY(), w, h, bodyType::STATIC);
	pbody->ctype = ColliderType::DIALOG_TRIGGER;
	pbody->listener = this;
	return true;
}

bool DialogTrigger::Update(float dt)
{
	if (triggered)
	{
		if(currentDialogDuration <= currentDialogTimer.ReadSec())
		{
			Engine::GetInstance().dialogManager->ShowDialogWindow(dt);
		}
		else if (currentDialogId != dialogues_ids.at(dialogues_ids.size() - 1))
		{
			currentDialog += 1;
			TriggerDialog(currentDialog);
			currentDialogTimer.Start();
		}
	}
	return true;
}

bool DialogTrigger::CleanUp()
{
	return true;
}

bool DialogTrigger::Destroy()
{
	active = false;

	if (this->tiledId != -1) {
		Engine::GetInstance().scene->destroyedEntitiesIds.push_back(this->tiledId);
	}

	pendingToDelete = true;
	return true;
}

void DialogTrigger::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (triggered) return;

	PhysBody* other = (physA == pbody) ? physB : physA;

	if (other->ctype == ColliderType::PLAYER)
	{
		Player* player = (Player*)other->listener;

		if (player != nullptr)
		{
			currentDialog = 0;
			TriggerDialog(dialogues_ids.at(currentDialog));
			triggered = true;
		}
	}
}

void DialogTrigger::TriggerDialog(int id)
{
	currentDialogId = id;
	currentDialogDuration = Engine::GetInstance().dialogManager->dialogs.at(id)->duration;
	Engine::GetInstance().dialogManager->LoadDialogWindow(currentDialogId);
}
