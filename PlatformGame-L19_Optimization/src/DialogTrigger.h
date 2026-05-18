#pragma once
#include "Entity.h"
#include "Textures.h"
#include "Animation.h"
#include "Physics.h"
#include "Scene.h"

class DialogTrigger : public Entity
{
public:
	DialogTrigger();
	virtual ~DialogTrigger();
	bool Awake();
	bool Start();
	bool Update(float dt);
	bool CleanUp();
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void SetPosition(Vector2D pos);
	Vector2D GetPosition();
	bool Destroy() override;
	void TriggerDialog(int id);

public:
	int w, h;
	bool triggered = false;
	int currentDialogId;
	int currentDialog;
	std::vector<int> dialogues_ids;
	Timer currentDialogTimer;
	int currentDialogDuration;

private:
	PhysBody* pbody;
};

