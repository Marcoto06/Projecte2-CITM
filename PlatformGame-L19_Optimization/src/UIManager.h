#pragma once

#include "Module.h"
#include "UIElement.h"

#include <list>

class UIManager : public Module
{
public:

	// Constructor
	UIManager();

	// Destructor
	virtual ~UIManager();
	
	// Called before the first frame
	bool Start();

	 // Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Additional methods
	std::shared_ptr<UIElement> CreateUIElement(UIElementType type, int id, const char* text, SDL_Rect bounds, Module* observer, SDL_Rect sliderBounds = { 0,0,0,0 });

	std::shared_ptr<UIElement> GetElement(int id);

	/* Pause Menu */
	void ShowPauseMenu();

	enum class MenuNavDirection
	{
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	void HandleUINavigation(int initialID, int finalID, MenuNavDirection direction);
	void HandlePauseMenuUIEvents(UIElement* uiElement);
	void LoadPauseMenu();
	void LoadPauseOptionsMenu();
	void LoadUITextures();

	/* Player UI */
	void ShowPlayerUI();

	/* GameOver UI */
	void ShowDeathScreen();
public:

	std::list<std::shared_ptr<UIElement>> UIElementsList;
	SDL_Texture* texture;
	int selectedUIID;

	/* Pause menu Textures */
	SDL_Texture* pauseOptionsMenuTexture = nullptr;
	SDL_Texture* sliderBoxTexture = nullptr;
	SDL_Texture* sliderAudioTexture = nullptr;
	SDL_Texture* deathScreenMenuTexture = nullptr;
	SDL_Texture* continuePauseButtonTexture = nullptr;
	SDL_Texture* optionsPauseButtonTexture = nullptr;
	SDL_Texture* menuQuitPauseButtonTexture = nullptr;
	SDL_Texture* gameQuitButtonTexture = nullptr;
	SDL_Texture* backButtonTexture = nullptr;

	/* Player UI Textures*/
	SDL_Texture* heartFullTexture = nullptr;
	SDL_Texture* heartHalfTexture = nullptr;
	SDL_Texture* heartEmptyTexture = nullptr;
	SDL_Texture* heartContainerTexture = nullptr;

	SDL_Texture* habilityContainerTexture = nullptr;
	SDL_Texture* habilityPowerJumpTexture = nullptr;

	/* GameOver Textures */
	SDL_Texture* gameOverTryAgainButtonTexture = nullptr;
	SDL_Texture* gameOverGoToMenuButtonTexture = nullptr;
};
