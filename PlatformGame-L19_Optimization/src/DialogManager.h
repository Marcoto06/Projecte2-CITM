#pragma once

#include "Module.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include "Vector2D.h"

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


	void ShowDialogWindow(int id);
	
private:
	SDL_Texture* dialogWindowTexture = NULL;
	int currentDialog;
	std::string dialogDatabase[];

};