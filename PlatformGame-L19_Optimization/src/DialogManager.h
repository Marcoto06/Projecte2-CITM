#pragma once

#include "Module.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include "Vector2D.h"


struct Dialog {
	int id;
	std::string name;
	std::string text;
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

	// Called before quitting
	bool CleanUp();


	bool Load(std::string path, std::string fileName);
	void ShowDialogWindow(int id);
	
	std::string dialogFileName;
	std::string dialogPath;

private:
	SDL_Texture* dialogWindowTexture = NULL;
	int currentDialog;
	Dialog dialogDatabase[100];
	pugi::xml_document dialogFileXML;

};