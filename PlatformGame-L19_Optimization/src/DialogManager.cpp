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

// 
bool DialogManager::Load(std::string path, std::string fileName)
{
	bool ret = false;
	dialogFileName = fileName;
	dialogPath = path;

	std::string dialogPathName = dialogPath + dialogFileName;

	pugi::xml_parse_result result = dialogFileXML.load_file(dialogPathName.c_str());
	
	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", dialogPathName.c_str(), result.description());
		ret = false;
	}
	else {

	}

	return ret;
}