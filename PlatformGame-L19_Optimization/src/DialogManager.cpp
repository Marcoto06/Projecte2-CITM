#include "Engine.h"
#include "DialogManager.h"
#include "Log.h"
#include "Textures.h"
#include "Render.h"
#include "Window.h"
#include "Audio.h"
#include "Scene.h"
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
	docTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Dialogs/Doc.png");
	lukeTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Dialogs/Luke.png");

	dialogPopFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx UI/rebre_missatge.wav");


	Load("Assets/Dialogos/", "Database.xml");
	spawnDialogPos = Vector2D(Engine::GetInstance().window->width / 2 - dialogWindowTexture->w / 2, 2000);
	dialogPos = Vector2D(Engine::GetInstance().window->width / 2 - dialogWindowTexture->w / 2, Engine::GetInstance().window->height - dialogWindowTexture->h);
	//LoadDialogWindow(0);
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

bool DialogManager::PostUpdate()
{
	if (!drawDialog) return true;
	DrawDialogWindow();
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
			if (dialog->name == "Doctor") {
				currentPortrait = docTexture;
			}
			else {
				currentPortrait = lukeTexture;
			}
			break;
		}
	}
	Engine::GetInstance().scene->player->list_dialogs.push_back(id);
	Engine::GetInstance().audio->PlayFx(dialogPopFxId);
}

void DialogManager::ShowDialogWindow(float dt) 
{
	//Box pop up
	if (currentDialogPos.getY() > dialogPos.getY())
	{
		currentDialogPos.setY(currentDialogPos.getY() - (dialogVelocity * dt));
	}
	else {
		currentDialogPos = dialogPos;
	}
}

void DialogManager::DrawDialogWindow() {
	//Draw box
	Engine::GetInstance().render->DrawTexture(dialogWindowTexture, currentDialogPos.getX(), currentDialogPos.getY(), NULL, 0.0f);
	Engine::GetInstance().render->DrawTexture(currentPortrait, currentDialogPos.getX() + 15, currentDialogPos.getY() + 7.5f, NULL, 0.0f);
	int nameWidth = currentDialog->name.size() * 25;
	Engine::GetInstance().render->DrawText(currentDialog->name.c_str(), currentDialogPos.getX() + 190, currentDialogPos.getY() + 45, nameWidth, 30, SDL_Color{ 255, 255, 255, 255 });
	//Draw the actual text, with multiple lines if needed
	int width = 20 * currentDialog->text.size();
	std::stringstream ss(currentDialog->text);

	int nLines = 1 + (width / 621);
	if (nLines > 1)
		width = 600;

	Engine::GetInstance().render->DrawText(currentDialog->text.c_str(), currentDialogPos.getX() + 190, currentDialogPos.getY() + 90, width, 30*nLines, SDL_Color{ 255,255,255,255 });

	/*while (std::getline(ss, line, '\n'))
	{
		int width = 25 * line.size();

		Engine::GetInstance().render->DrawText(line.c_str(), currentDialogPos.getX() + 190, currentDialogPos.getY() + 90 + yOffset, width, 30, SDL_Color{ 255,255,255,255 });

		yOffset += 35;
	}*/
}