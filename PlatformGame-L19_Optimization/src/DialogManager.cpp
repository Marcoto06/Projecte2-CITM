#include "Engine.h"
#include "DialogManager.h"
#include "Log.h"
#include "Textures.h"
#include "Render.h"



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
	dialogWindowTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Dialogs/Dialog_box.png");
	Load("Assets/Dialogos/", "Database.xml");
	ShowDialogWindow(0);
	return true;
}

// Called each loop iteration
bool DialogManager::PreUpdate()
{
	

	return true;
}

bool DialogManager::Update(float dt)
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
		for (const auto& child : dialogFileXML.child("Dialogs")) {
			Dialog* dialog = new Dialog(child.attribute("id").as_int(), child.attribute("name").as_string(), child.text().as_string(), child.attribute("duration").as_int());
			dialogs.push_back(dialog);
		}
	}

	return ret;
}

void DialogManager::LoadDialogWindow(int id) {
	for (const auto& dialog : dialogs) {
		if (dialog->id == id) {
			currentDialog = dialog;
			currentDialogPos = spawnDialogPos;
		}
	}
}

void DialogManager::ShowDialogWindow(float dt) 
{
	if (currentDialogPos.getY() > dialogPos.getY())
	{
		currentDialogPos.setY(currentDialogPos.getY() - (dialogVelocity * dt));
	}
	Engine::GetInstance().render->DrawTexture(dialogWindowTexture, currentDialogPos.getX(), currentDialogPos.getY(), NULL, 0.0f);
	//Engine::GetInstance().render->DrawText(currentDialog->text.c_str(), currentDialogPos.getX(), currentDialogPos.getY(), 500, 500);
}