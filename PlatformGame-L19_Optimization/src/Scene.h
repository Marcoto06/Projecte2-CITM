#pragma once

#include "Module.h"
#include "Player.h"
#include "UIButton.h"
#include "pl_mpeg.h"
#include <cstdio>
#include <memory>

struct SDL_Texture;
class Player;

//L17 TODO 1: Define SceneID enum class with INTRO_SCREEN, MAIN_MENU, LEVEL1, LEVEL2
enum class SceneID
{
	INTRO_SCREEN,
	MAIN_MENU,
	LEVEL1,
	LEVEL2
};

enum class MainMenuState
{
	MAIN_BUTTONS,
	OPTIONS
};

enum class PauseMenuState
{
	NOTPAUSED,
	MAIN,
	OPTIONS,
	QUIT_CONFIRM,
	INVENTORY
};

enum class MenuNavDirection
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Return the player position
	Vector2D GetPlayerPosition();

	// Get tilePosDebug value
	std::string GetTilePosDebug() {
		return tilePosDebug;
	}

	// Handles multiple Gui Event methods
	bool OnUIMouseClickEvent(UIElement* uiElement);

	// L17 TODO 2: Define functions to handle scene changes
	void ChangeScene(SceneID newScene);
	void UnloadCurrentScene();
	void LoadScene(SceneID newScene);

	void ActivateGameOver();
	std::shared_ptr<Player> player;

	std::vector<int> destroyedEntitiesIds;

	void SaveGame();
	void LoadGame();

private:

	void PlayIntroVideo();
	void StopIntroVideo();

	static void OnVideoFrame(plm_t* mpeg, plm_frame_t* frame, void* user);	//Callback function required by pl_mpeg to return video frames

	// L17 TODO 3: Define specific function for main menu scene: Load, Unload, Handle UI events
	void LoadMainMenu();
	void UnloadMainMenu();
	void UpdateMainMenu(float dt);
	void ShowMainMenuButtons();
	void LoadOptionsMainMenu();
	void HandleMainMenuUIEvents(UIElement* uiElement);

	void HandlePauseMenuUIEvents(UIElement* uiElement);
	void HandlePause();
	void LoadPauseMenu();
	void UnloadPauseMenu();
	void LoadPauseOptionsMenu();
	void LoadInventoryMenu();
	void HandleUINavigation(int initialID, int finalID, MenuNavDirection direction);

	// L17 TODO 4: Define specific functions for level1 scene: Load, Unload, Update, PostUpdate
	void LoadLevel1();
	void UnloadLevel1();
	void UpdateLevel1(float dt);
	void PostUpdateLevel1();

	// L17 TODO 5: Define specific functions for level2 scene: Load, Unload, Update
	void LoadLevel2();
	void UpdateLevel2(float dt);
	void UnloadLevel2();

	void ShowDeathScreen();

private:

	struct VideoData {
		SDL_Texture* texture = nullptr;
		uint8_t* buffer = nullptr;
		int width = 0;
		int height = 0;
	};

	VideoData introVideo;
	plm_t* plm = nullptr;      // Principal pointer to the library
	bool isPlayingVideo = false;

	//L03: TODO 3b: Declare a Player attribute
	SDL_Texture* mouseTileTex = nullptr;
	std::string tilePosDebug = "[0,0]";
	bool once = false;

	// L16: TODO 2: Declare a UIButton 
	std::shared_ptr<UIButton> uiBt;
	float volume = 1.0;

	// L17 TODO 1: Current scene attribute with initial value
	SceneID currentScene = SceneID::MAIN_MENU;
	MainMenuState currentMenuState = MainMenuState::MAIN_BUTTONS;
	PauseMenuState currentPauseState = PauseMenuState::NOTPAUSED;

	SDL_Texture* mainMenuBackground = nullptr;

	SDL_Texture* playButtonTexture = nullptr;
	SDL_Texture* optionsButtonTexture = nullptr;
	SDL_Texture* exitButtonTexture = nullptr;

	SDL_Texture* backButtonTexture = nullptr;

	SDL_Texture* continuePauseButtonTexture = nullptr;
	SDL_Texture* optionsPauseButtonTexture = nullptr;
	SDL_Texture* menuQuitPauseButtonTexture = nullptr;
	SDL_Texture* gameQuitButtonTexture = nullptr;

	SDL_Texture* pauseOptionsMenuTexture = nullptr;

	SDL_Texture* sliderBoxTexture = nullptr;
	SDL_Texture* sliderAudioTexture = nullptr;
	SDL_Texture* sliderBarTexture = nullptr;
	SDL_Texture* sliderKnobTexture = nullptr;

	SDL_Texture* heartFullTexture = nullptr;
	SDL_Texture* heartHalfTexture = nullptr;
	SDL_Texture* heartEmptyTexture = nullptr;
	SDL_Texture* heartContainerTexture = nullptr;

	SDL_Texture* habilityContainerTexture = nullptr;
	SDL_Texture* habilityPowerJumpTexture = nullptr;

	SDL_Texture* deathScreenMenuTexture = nullptr;

	SDL_Texture* gameOverTryAgainButtonTexture = nullptr;
	SDL_Texture* gameOverGoToMenuButtonTexture = nullptr;

	bool isGameOver = false;
	int selectedUIID;
};