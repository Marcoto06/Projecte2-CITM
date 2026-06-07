#pragma once

#include "Module.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include "Vector2D.h"
#include <vector>
using namespace std;


struct Dialog {
	int id;
	std::string name;
	std::string text;
	int duration;
	Dialog(int _id, std::string _name, std::string _text, int _duration) : id(_id), name(_name), text(_text), duration(_duration) {}
};


class DialogManager : public Module
{

public:

	DialogManager();

	// Destructor
	virtual ~DialogManager();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();


	bool Load(std::string path, std::string fileName);
	void LoadDialogWindow(int id);
	void ShowDialogWindow(float dt);
	void DrawDialogWindow();
	
	std::string dialogFileName;
	std::string dialogPath;
	std::vector<Dialog*> dialogs;

	bool drawDialog = false;

	//audiofx
	int dialogPopFxId;


private:
	SDL_Texture* dialogWindowTexture = NULL;
	SDL_Texture* docTexture = NULL;
	SDL_Texture* lukeTexture = NULL;
	SDL_Texture* currentPortrait = NULL;
	
	pugi::xml_document dialogFileXML;
	
	Dialog* currentDialog;

	Vector2D dialogPos;
	Vector2D currentDialogPos;
	Vector2D spawnDialogPos;
	int dialogVelocity = 3;
};