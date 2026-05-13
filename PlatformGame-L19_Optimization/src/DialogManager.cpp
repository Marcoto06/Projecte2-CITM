#include "Engine.h"
#include "DialogManager.h"
#include "Log.h"

DialogManager::DialogManager() : Module()
{
	
}

// Destructor
DialogManager::~DialogManager()
{
	
}

// Called before render is available
bool DialogManager::Awake()
{
	bool ret = true;
	
	

	return ret;
}

// Called before the first frame
bool DialogManager::Start()
{
	//dialogWindowTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/menu INCORPUS.png");

	return true;
}

// Called each loop iteration
bool DialogManager::PreUpdate()
{
	

	return true;
}

// Called before quitting
bool DialogManager::CleanUp()
{
	
	return true;
}
