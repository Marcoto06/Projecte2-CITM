#include "Engine.h"
#include "DialogManager.h"
#include "Log.h"
#include "Textures.h"
#include "Render.h"
#include <sstream>//Necessary to write multiple lines.



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
	LoadDialogWindow(0);
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
		LOG("Could not load dialog xml file %s. pugi error: %s", dialogPathName.c_str(), result.description());
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
	//Box pop up
	if (currentDialogPos.getY() > dialogPos.getY())
	{
		currentDialogPos.setY(currentDialogPos.getY() - (dialogVelocity * dt));
	}
	//Draw box
	Engine::GetInstance().render->DrawTexture(dialogWindowTexture, currentDialogPos.getX(), currentDialogPos.getY(), NULL, 0.0f);
	int nameWidth = currentDialog->name.size() * 25;
	Engine::GetInstance().render->DrawText(currentDialog->name.c_str(), currentDialogPos.getX() + 190, currentDialogPos.getY() + 45, nameWidth, 30, SDL_Color{ 255, 255, 255, 255 });
	//Draw the actual text, with multiple lines if needed
	int width = 25 * currentDialog->text.size();
	std::stringstream ss(currentDialog->text);
	std::string line;

	int yOffset = 0;

	while (std::getline(ss, line, '\n'))
	{
		int width = 25 * line.size();

		Engine::GetInstance().render->DrawText(line.c_str(), currentDialogPos.getX() + 190, currentDialogPos.getY() + 90 + yOffset, width, 30, SDL_Color{ 255,255,255,255 });

		yOffset += 35;
	}
}