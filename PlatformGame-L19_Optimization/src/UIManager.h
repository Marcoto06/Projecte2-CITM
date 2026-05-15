#pragma once

#include "Module.h"
#include "UIElement.h"
#include "Player.h"

#include <list>

class Player;

class UIManager : public Module
{
public:

	// Constructor
	UIManager();

	// Destructor
	virtual ~UIManager();
	
	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	 // Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Additional methods
	std::shared_ptr<UIElement> CreateUIElement(UIElementType type, int id, const char* text, SDL_Rect bounds, Module* observer, SDL_Rect sliderBounds = { 0,0,0,0 });

	std::shared_ptr<UIElement> GetElement(int id);

	enum class PauseMenuState
	{
		NOTPAUSED,
		MAIN,
		OPTIONS,
		QUIT_CONFIRM,
		INVENTORY
	};

	enum class MainMenuState
	{
		MAIN_BUTTONS,
		PLAY_MENU,
		OPTIONS
	};

	MainMenuState currentMenuState = MainMenuState::MAIN_BUTTONS;;
	PauseMenuState currentPauseState = PauseMenuState::NOTPAUSED;;

	/* Main Menu */
	void LoadMainMenuButtons();
	void ShowMainMenuButtons();
	void LoadOptionsMainMenu();
	void LoadPlayMainMenu();
	void HandleMainMenuUIEvents(UIElement* uiElement);

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
	void changeLifeAnim(int life);

	/* GameOver UI */
	void ShowDeathScreen();
public:

	std::list<std::shared_ptr<UIElement>> UIElementsList;
	SDL_Texture* texture;
	int selectedUIID;

	/* Main menu Textures*/
	SDL_Texture* mainMenuBackground = nullptr;
	SDL_Texture* playButtonTexture = nullptr;
	SDL_Texture* newgameButtonTexture = nullptr;
	SDL_Texture* optionsButtonTexture = nullptr;
	SDL_Texture* exitButtonTexture = nullptr;
	SDL_Texture* sliderBarTexture = nullptr;
	SDL_Texture* sliderKnobTexture = nullptr;

	/* Pause menu Textures */
	SDL_Texture* pauseOptionsMenuTexture = nullptr;
	SDL_Texture* sliderBoxTexture = nullptr;
	SDL_Texture* sliderMusicTexture = nullptr;
	SDL_Texture* sliderSFXTexture = nullptr;
	SDL_Texture* deathScreenMenuTexture = nullptr;
	SDL_Texture* continuePauseButtonTexture = nullptr;
	SDL_Texture* optionsPauseButtonTexture = nullptr;
	SDL_Texture* menuQuitPauseButtonTexture = nullptr;
	SDL_Texture* gameQuitButtonTexture = nullptr;
	SDL_Texture* backButtonTexture = nullptr;

	/* Player UI Textures*/
	/*SDL_Texture* heartFullTexture = nullptr;
	SDL_Texture* heartHalfTexture = nullptr;
	SDL_Texture* heartEmptyTexture = nullptr;
	SDL_Texture* heartContainerTexture = nullptr;*/
	AnimationSet life_anims;
	SDL_Texture* lifeTexture = nullptr;

	SDL_Texture* habilityContainerTexture = nullptr;
	SDL_Texture* habilityPowerJumpTexture = nullptr;

	/* GameOver Textures */
	SDL_Texture* gameOverTryAgainButtonTexture = nullptr;
	SDL_Texture* gameOverGoToMenuButtonTexture = nullptr;

private:

	std::shared_ptr<Player> player;
};
