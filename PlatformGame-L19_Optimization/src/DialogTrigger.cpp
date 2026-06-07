#include "DialogTrigger.h"
#include "Player.h"
#include "Engine.h"
#include "Audio.h"
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
		if(currentDialogDuration >= currentDialogTimer.ReadSec() || currentDialogDuration == 0)
		{
			Engine::GetInstance().dialogManager->ShowDialogWindow(dt);
		}
		else if (currentDialogId == dialogues_ids.at(dialogues_ids.size() - 1)) {
			Engine::GetInstance().dialogManager->drawDialog = false;
			player->lock = false;
		}
		if ((currentDialogDuration != 0 && currentDialogDuration < currentDialogTimer.ReadSec() && currentDialogId != dialogues_ids.at(dialogues_ids.size() - 1))|| (currentDialogDuration == 0 && (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_SOUTH) == KEY_DOWN)))
		{
			currentDialogId += 1;
			TriggerDialog(currentDialogId);
		}
	}
	return true;
}

bool DialogTrigger::CleanUp()
{
	Engine::GetInstance().physics->DeletePhysBody(pbody);
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
	/*if (triggered) return;

	PhysBody* other = (physA == pbody) ? physB : physA;

	if (other->ctype == ColliderType::PLAYER)
	{
		player = (Player*)other->listener;

		if (player != nullptr)
		{
			player->lock = lock;
			TriggerDialog(dialogues_ids.at(0));
			triggered = true;
		}
	}*/
}

void DialogTrigger::TriggerDialog(int id)
{
	currentDialogId = id;
	currentDialogDuration = Engine::GetInstance().dialogManager->dialogs.at(id)->duration;
	Engine::GetInstance().dialogManager->LoadDialogWindow(currentDialogId);
	Engine::GetInstance().dialogManager->drawDialog = true;
	currentDialogTimer.Start();
}
