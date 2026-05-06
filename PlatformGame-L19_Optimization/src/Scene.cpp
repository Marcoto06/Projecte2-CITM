#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"
#include "Enemy.h"
#include "Checkpoint.h"
#include "UIManager.h"
#include "UISlider.h"
#include "UICheckBox.h"

Scene::Scene() : Module()
{
	name = "scene";
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	LoadScene(currentScene); // start in MAIN_MENU
	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	if (isPlayingVideo) {
		
		plm_decode(plm, dt / 1000.0f);	// pl_mpeg uses time in seconds, dt is in milliseconds

		if (introVideo.texture && introVideo.buffer) {
			SDL_UpdateTexture(introVideo.texture, NULL, introVideo.buffer, introVideo.width * 4);

			SDL_RenderTexture(Engine::GetInstance().render->renderer, introVideo.texture, NULL, NULL);
		}

		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || plm_has_ended(plm))	// If space is pressed or the video has ended, stop the video and load the main menu
		{
			StopIntroVideo();
			ChangeScene(SceneID::LEVEL1);
		}

		return true;
	}

	switch (currentScene)
	{
	case SceneID::INTRO_SCREEN:
		break;
	case SceneID::MAIN_MENU:
		UpdateMainMenu(dt);
		break;
	case SceneID::LEVEL1:
		UpdateLevel1(dt);
		break;
	case SceneID::LEVEL2:
		UpdateLevel2(dt);
		break;
	}

	return true;
}

bool Scene::PostUpdate()
{
	bool ret = true;

	switch (currentScene)
	{
	case SceneID::INTRO_SCREEN:
		break;
	case SceneID::MAIN_MENU:
		break;
	case SceneID::LEVEL1:
		PostUpdateLevel1();
		break;
	case SceneID::LEVEL2:
		break;
	default:
		break;
	}

	/*if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;*/

	if (Engine::GetInstance().quit == true)
		ret = false;

	return ret;
}

bool Scene::OnUIMouseClickEvent(UIElement* uiElement)
{
	switch (currentScene)
	{
	case SceneID::INTRO_SCREEN:
		break;
	case SceneID::MAIN_MENU:
		HandleMainMenuUIEvents(uiElement);
		break;
	case SceneID::LEVEL1:
		HandlePauseMenuUIEvents(uiElement);
		break;
	case SceneID::LEVEL2:
		HandlePauseMenuUIEvents(uiElement);
		break;
	default:
		break;
	}

	return true;
}

bool Scene::CleanUp()
{
	LOG("Freeing scene");
	UnloadCurrentScene();
	return true;
}

Vector2D Scene::GetPlayerPosition()
{
	if (player && player->active)
	{
		return player->GetPosition();
	}

	return Vector2D(0, 0);
}

// *********************************************
// Scene change functions
// *********************************************

void Scene::LoadScene(SceneID newScene)
{
	auto& engine = Engine::GetInstance();

	switch (newScene)
	{
	case SceneID::MAIN_MENU:
		LoadMainMenu();
		break;

	case SceneID::LEVEL1:
		LoadLevel1();
		break;

	case SceneID::LEVEL2:
		LoadLevel2();
		break;
	}
}

void Scene::ChangeScene(SceneID newScene)
{
	UnloadCurrentScene();
	currentScene = newScene;
	LoadScene(currentScene);
}

void Scene::UnloadCurrentScene() {

	switch (currentScene)
	{
	case SceneID::MAIN_MENU:
		UnloadMainMenu();
		break;

	case SceneID::LEVEL1:
		UnloadLevel1();
		break;

	case SceneID::LEVEL2:
		UnloadLevel2();
		break;
	}
}

// *********************************************
// MAIN MENU functions
// *********************************************

void Scene::LoadMainMenu() {

	//Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/retro-gaming-short-248416.wav");

	/* Load all needed textures */

	mainMenuBackground = Engine::GetInstance().textures->Load("Assets/Textures/UI/menu INCORPUS.png");
	sliderBarTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderBar.png");
	sliderKnobTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderKnob.png");
	backButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/BackButton.png");
	playButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/PlayButton.png");
	optionsButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/OptionsButton.png");
	exitButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/ExitButton.png");
	sliderBoxTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderBox.png");
	sliderAudioTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/AudioIcon.png");
	
	ShowMainMenuButtons();
}

void Scene::ShowMainMenuButtons() 
{
	Engine::GetInstance().uiManager->CleanUp();

	currentMenuState = MainMenuState::MAIN_BUTTONS;

	mainMenuBackground = Engine::GetInstance().textures->Load("Assets/Textures/UI/fondo_menu (con titulo).png");

	int screenWidth, screenHeight;
	Engine::GetInstance().window->GetWindowSize(screenWidth, screenHeight);

	int buttonWidth = 290;
	int buttonHeight = 86;

	// Instantiate a UIButton in the Scene
	SDL_Rect playButtonRect = { 146 , 412 , buttonWidth, buttonHeight };
	SDL_Rect optionsButtonRect = { 149 , 578 , buttonWidth, buttonHeight };
	SDL_Rect exitButtonRect = { 148 , 759 , buttonWidth, buttonHeight };

	auto playButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, " ",playButtonRect, this);
	playButton->SetTexture(playButtonTexture);
	playButton->isSelected = true;
	selectedUIID = 1;

	auto optionsButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, " ", optionsButtonRect, this);
	optionsButton->SetTexture(optionsButtonTexture);

	auto exitButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 3, " ", exitButtonRect, this);
	exitButton->SetTexture(exitButtonTexture);
}

void Scene::UnloadMainMenu() {
	// Clean up UI elements related to the main menu
	Engine::GetInstance().uiManager->CleanUp();	

	if (mainMenuBackground != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(mainMenuBackground);
		mainMenuBackground = nullptr;
	}
}

void Scene::UpdateMainMenu(float dt) {
	int firstElement = 0;
	int lastElement = 0;

	if (mainMenuBackground != nullptr)
	{
		Engine::GetInstance().render->DrawTexture(mainMenuBackground, 0, 0, NULL, 0.0f);
	}

	if (currentMenuState == MainMenuState::OPTIONS) {
		int w, h;
		
		Engine::GetInstance().window->GetWindowSize(w, h);

		SDL_Rect fullscreenRect = { 0, 0, w, h };

		Engine::GetInstance().render->DrawRectangle(fullscreenRect, 0, 0, 0, 150, true, false);

		Engine::GetInstance().render->DrawTexture(sliderBoxTexture, (w - sliderBoxTexture->w) / 2, (h - (sliderBoxTexture->h * 2)) / 2, NULL, 0.0f);
		Engine::GetInstance().render->DrawTexture(sliderAudioTexture, ((w - sliderAudioTexture->w) / 2) - 200, ((h - sliderAudioTexture->h) / 2) - 65, NULL, 0.0f);

		Engine::GetInstance().render->DrawTexture(sliderBoxTexture, (w - sliderBoxTexture->w) / 2, ((h - (sliderBoxTexture->h)) / 2) + 100, NULL, 0.0f);
		Engine::GetInstance().render->DrawTexture(sliderAudioTexture, ((w - sliderAudioTexture->w) / 2) - 200, ((h - sliderAudioTexture->h) / 2) + 100, NULL, 0.0f);

		firstElement = 4;
		lastElement = 7;
		
	}
	else {
		firstElement = 1;
		lastElement = 3;
	}

	/* UI CONTROLS */
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_DPAD_UP) == KEY_DOWN)
		HandleUINavigation(firstElement, lastElement, MenuNavDirection::UP);
	else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_DPAD_DOWN) == KEY_DOWN)
		HandleUINavigation(firstElement, lastElement, MenuNavDirection::DOWN);
	else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT || Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_DPAD_LEFT) == KEY_DOWN)
		HandleUINavigation(firstElement, lastElement, MenuNavDirection::LEFT);
	else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT || Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_DPAD_RIGHT) == KEY_DOWN)
		HandleUINavigation(firstElement, lastElement, MenuNavDirection::RIGHT);

}

void Scene::HandleMainMenuUIEvents(UIElement* uiElement)
{
	switch (uiElement->id)
	{
	case 1: // Play Button

		if (std::remove("Saves/savegame.xml") == 0) 
		{
			LOG("savegame.xml deleted successfully.");
		}
		else 
		{
			LOG("savegame.xml not found or could not be deleted.");
		}

		// INTRO VIDEO DISABLER
		// UNCOMMENT & COMMENT THE LINES BELOW TO SKIP THE INTRO VIDEO AND GO DIRECTLY TO THE MAIN MENU

		ChangeScene(SceneID::LEVEL1);
		//PlayIntroVideo();
		break;
	case 2: // Button Options
		LOG("Main Menu: Options button clicked!");
		LoadOptionsMainMenu();
		break;
	case 3: // Button Exit
		LOG("Main Menu: Exit clicked!");
		Engine::GetInstance().quit = true;
		break;
	case 4: // Fullscreen Toggle
	{
		Engine::GetInstance().render->ToggleFullScreen();
		UICheckBox* check = static_cast<UICheckBox*>(uiElement);
		bool value = Engine::GetInstance().render->IsFullScreen();
		check->SetValue(value);
		break;
	}
	case 5:
	{
		UISlider * slider = static_cast<UISlider*>(uiElement);
		float volume = slider->GetValue() / 100.0f;
		Engine::GetInstance().audio->SetMusicVolume(volume);
		break;
	}
	case 6:
	{
		UISlider* slider = static_cast<UISlider*>(uiElement);
		float volume = slider->GetValue() / 100.0f;
		Engine::GetInstance().audio->SetSFXVolume(volume);
		break;
	}
	case 7:
		LOG("Main Menu: Back button clicked!");
		ShowMainMenuButtons();
		break;
	default:
		break;
	}
}

// *********************************************
// OPTIONS MAIN MENU functions
// *********************************************

void Scene::LoadOptionsMainMenu() 
{
	
	Engine::GetInstance().uiManager->CleanUp();

	currentMenuState = MainMenuState::OPTIONS;

	int screenWidth, screenHeight;
	Engine::GetInstance().window->GetWindowSize(screenWidth, screenHeight);

	SDL_Rect checkBoxPos = { ((screenWidth - sliderBarTexture->w) / 2) + 50, (screenHeight / 2) - 200, 40, 40 };
	auto fullcreenElement = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::CHECKBOX, 4, " FULLSCREEN ", checkBoxPos, this);
	fullcreenElement->isSelected = true;
	selectedUIID = 4;

	SDL_Rect sliderBounds = { ((screenWidth - sliderBarTexture->w) / 2) + 50, (screenHeight / 2) - 80, 399, 25 };
	auto musicSliderElement = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::SLIDER, 5, "MUSIC", sliderBounds, this);

	SDL_Rect fxSliderBounds = { ((screenWidth - sliderBarTexture->w) / 2) + 50, (screenHeight / 2) + 90, 399, 25 };
	auto fxSliderElement = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::SLIDER, 6, "FX", fxSliderBounds, this);

	auto musicSlider = std::static_pointer_cast<UISlider>(musicSliderElement);
	musicSlider->SetTexture(sliderBarTexture);
	musicSlider->SetKnobTexture(sliderKnobTexture);

	auto fxSlider = std::static_pointer_cast<UISlider>(fxSliderElement);
	fxSlider->SetTexture(sliderBarTexture);
	fxSlider->SetKnobTexture(sliderKnobTexture);


	SDL_Rect backButtonRect = { (screenWidth - backButtonTexture->w) / 2, 736, 290, 86};

	auto backButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 7, " ", backButtonRect, this);
	backButton->SetTexture(backButtonTexture);
}	

// *********************************************
// PAUSE MENU functions
// *********************************************

void Scene::HandlePauseMenuUIEvents(UIElement* uiElement)
{
	switch (uiElement->id)
	{
	case 1: // PAUSE MENU: CONTINUE
		LOG("PAUSE MENU: CONTINUE clicked!");
		Engine::GetInstance().Func_PauseEngine();
		Engine::GetInstance().uiManager->CleanUp();
		break;
	case 2: // PAUSE MENU: OPTIONS
		LoadPauseOptionsMenu();
		break;
	case 3: // PAUSE MENU: QUIT
		Engine::GetInstance().uiManager->CleanUp();
		ChangeScene(SceneID::MAIN_MENU);
		Engine::GetInstance().Func_PauseEngine();
		break;
	case 4: // PAUSE MENU: QUIT to Main Menu
		Engine::GetInstance().quit = true;
		break;
	case 5: // PAUSE MENU OPTIONS: Fullscreen Toggle
	{
		Engine::GetInstance().render->ToggleFullScreen();
		UICheckBox* check = static_cast<UICheckBox*>(uiElement);
		bool value = Engine::GetInstance().render->IsFullScreen();
		check->SetValue(value);

		break;
	}
	case 6: // PAUSE MENU OPTIONS: Music Slider
	{
		UISlider* slider = static_cast<UISlider*>(uiElement);
		float volume = slider->GetValue() / 100.0f;
		Engine::GetInstance().audio->SetMusicVolume(volume);
		break;
	}
	case 7: // PAUSE MENU OPTIONS: SFX Slider
	{
		UISlider* slider = static_cast<UISlider*>(uiElement);
		float volume = slider->GetValue() / 100.0f;
		Engine::GetInstance().audio->SetSFXVolume(volume);
		break;
	}
	case 8: // PAUSE MENU OPTIONS: Back Button
	{
		Engine::GetInstance().uiManager->CleanUp();
		LoadPauseMenu();
		break;
	}
	case 10: //	DEATH SCREEN: Try Again Button
	{
		isGameOver = false;
		Engine::GetInstance().paused = false;
		Engine::GetInstance().uiManager->CleanUp();
		ChangeScene(currentScene);
		break;
	}
	case 11: //	DEATH SCREEN: Go To Menu Button
	{
		isGameOver = false;
		Engine::GetInstance().paused = false;
		Engine::GetInstance().uiManager->CleanUp();
		ChangeScene(SceneID::MAIN_MENU);
		break;
	}
	}
}

void Scene::HandlePause() {
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_START) == KEY_DOWN) {
		Engine::GetInstance().Func_PauseEngine();

		if (Engine::GetInstance().paused) {
			LoadPauseMenu();
		}
		else {
			UnloadMainMenu();
		}
	}
	
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
	{
		Engine::GetInstance().Func_PauseEngine();

		if (Engine::GetInstance().paused) {
			LoadInventoryMenu();
		}
		else {
			UnloadMainMenu();
		}
	}
}

void Scene::LoadPauseMenu() {
	Engine::GetInstance().uiManager->CleanUp();
	currentPauseState = PauseMenuState::MAIN;

	int button_width = 290;
	int button_height = 86;
	int button_margin = 20;

	int center_window_posX = (Engine::GetInstance().window->width / 2) - button_width / 2;
	int center_window_posY = Engine::GetInstance().window->height / 2;

	pauseOptionsMenuTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Fondo_pause_menu.png");
		
	continuePauseButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/ContinueButton.png");
	optionsPauseButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/OptionsButton.png");
	menuQuitPauseButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/QuitToMenuButton.png");
	gameQuitButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/QuitGameButton.png");

	SDL_Rect continueButtonRect = { center_window_posX, center_window_posY - (button_height * 2) - (button_margin * 2), button_width, button_height };
	SDL_Rect optionsButtonRect = { center_window_posX, continueButtonRect.y + button_height + button_margin, button_width, button_height };
	SDL_Rect quitToMenuButtonRect = { center_window_posX, optionsButtonRect.y + button_height + button_margin, button_width, button_height };
	SDL_Rect quitToDesktopButtonRect = { center_window_posX, quitToMenuButtonRect.y + button_height + button_margin, button_width, button_height };

	auto continueButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, " CONTINUE ", continueButtonRect, this);
	continueButton->SetTexture(continuePauseButtonTexture);
	continueButton->isSelected = true;
	selectedUIID = 1;

	auto optionsButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, " OPTIONS ", optionsButtonRect, this);
	optionsButton->SetTexture(optionsPauseButtonTexture);

	auto quitToMenuButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 3, " QUIT TO MENU ", quitToMenuButtonRect, this);
	quitToMenuButton->SetTexture(menuQuitPauseButtonTexture);

	auto quitToDesktopButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 4, " QUIT TO DESKTOP ", quitToDesktopButtonRect, this);
	quitToDesktopButton->SetTexture(gameQuitButtonTexture);
}

void Scene::LoadPauseOptionsMenu() 
{
	Engine::GetInstance().uiManager->CleanUp();
	currentPauseState = PauseMenuState::OPTIONS;

	int button_width = 290;
	int button_height = 86;
	int button_margin = 20;

	int center_window_posX = (Engine::GetInstance().window->width / 2) - button_width / 2;
	int center_window_posY = Engine::GetInstance().window->height / 2;

	SDL_Rect checkBoxPos = { center_window_posX - 130, center_window_posY - 200, 40, 40 };
	auto fullcreenElement = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::CHECKBOX, 5, " FULLSCREEN ", checkBoxPos, this);
	fullcreenElement->isSelected = true;
	selectedUIID = 5;

	SDL_Texture* sliderBarTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderBar.png");
	SDL_Texture* sliderKnobTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderKnob.png");

	SDL_Rect musicSliderBounds = { center_window_posX, center_window_posY - 80, 399, 25 };
	auto musicSliderElement = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::SLIDER, 6, " MUSIC ", musicSliderBounds, this);
	
	SDL_Rect sfxSliderBounds = { center_window_posX, center_window_posY + 75, 399, 25 };
	auto sfxSliderElement = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::SLIDER, 7, " SFX ", sfxSliderBounds, this);

	auto musicSlider = std::static_pointer_cast<UISlider>(musicSliderElement);
	musicSlider->SetTexture(sliderBarTexture);
	musicSlider->SetKnobTexture(sliderKnobTexture);

	auto sfxSlider = std::static_pointer_cast<UISlider>(sfxSliderElement);
	sfxSlider->SetTexture(sliderBarTexture);
	sfxSlider->SetKnobTexture(sliderKnobTexture);

	backButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/BackButton.png");

	SDL_Rect backButtonPos = { center_window_posX, center_window_posY + 175, 290, 86};

	auto backButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 8, " ", backButtonPos, this);
	backButton->SetTexture(backButtonTexture);
}

void Scene::LoadInventoryMenu()
{
	Engine::GetInstance().uiManager->CleanUp();
	currentPauseState = PauseMenuState::INVENTORY;	

	pauseOptionsMenuTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Fondo_pause_menu.png");
}

void Scene::UnloadPauseMenu()
{

}

// *********************************************
// Level 1 functions
// *********************************************

void Scene::LoadLevel1() {

	//Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/level-iv-339695.wav");

	//Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");

	//Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player);

	heartFullTexture= Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Corazon_full.png");
	heartHalfTexture= Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Corazon_meitat.png");
	heartEmptyTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Corazon_muerto.png");
	heartContainerTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Caja_vida.png");
	habilityContainerTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Hueco_habilidades_vacio.png");
	habilityPowerJumpTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Hueco_habilidades_Salto.png");
	sliderBoxTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderBox.png");
	sliderAudioTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/AudioIcon.png");
	deathScreenMenuTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/DeathMenu/Fondo_death_menu.png");
	gameOverTryAgainButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/DeathMenu/TryAgainButton.png");
	gameOverGoToMenuButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/DeathMenu/GoToMenuButton.png");

	LoadGame();
}

void Scene::UpdateLevel1(float dt) {
	Engine::GetInstance().map->DrawForeground();

	HandlePause();

	if (Engine::GetInstance().paused) {
		int w, h;
		int firstElement = 0;
		int lastElement = 0;
		
		Engine::GetInstance().window->GetWindowSize(w, h);
		SDL_Rect fullscreenRect = { 0, 0, w, h };

		Engine::GetInstance().render->DrawRectangle(fullscreenRect, 0, 0, 0, 150, true, false);

		int imgWidth = 855;
		int imgHeight = 839;

		int pauseOptionsMenuPosX = (w - imgWidth) / 2;
		int pauseOptionsMenuPosY = (h - imgHeight) / 2;

		Engine::GetInstance().render->DrawTexture(pauseOptionsMenuTexture, pauseOptionsMenuPosX, pauseOptionsMenuPosY, NULL, 0.0f);

		if (currentPauseState == PauseMenuState::OPTIONS)
		{
			Engine::GetInstance().render->DrawTexture(sliderBoxTexture, (w - sliderBoxTexture->w) / 2, (h - (sliderBoxTexture->h * 2)) / 2, NULL, 0.0f);
			Engine::GetInstance().render->DrawTexture(sliderAudioTexture, ((w - sliderAudioTexture->w) / 2) - 200, ((h - sliderAudioTexture->h) / 2) - 65, NULL, 0.0f);

			Engine::GetInstance().render->DrawTexture(sliderBoxTexture, (w - sliderBoxTexture->w) / 2, ((h - (sliderBoxTexture->h)) / 2) + 80, NULL, 0.0f);
			Engine::GetInstance().render->DrawTexture(sliderAudioTexture, ((w - sliderAudioTexture->w) / 2) - 200, ((h - sliderAudioTexture->h) / 2) + 80, NULL, 0.0f);

			firstElement = 5;
			lastElement = 8;
		}
		else {
			firstElement = 1;
			lastElement = 4;
		}

		if (isGameOver)
		{
			Engine::GetInstance().render->DrawRectangle(fullscreenRect, 0, 0, 0, 150, true, false);
			Engine::GetInstance().render->DrawTexture(deathScreenMenuTexture, (w - deathScreenMenuTexture->w) / 2, (h -deathScreenMenuTexture->h) / 2, NULL, 0.0f);

		}

		/* UI CONTROLS */
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_DPAD_UP) == KEY_DOWN)
			HandleUINavigation(firstElement, lastElement, MenuNavDirection::UP);
		else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_DPAD_DOWN) == KEY_DOWN)
			HandleUINavigation(firstElement, lastElement, MenuNavDirection::DOWN);
		else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT || Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_DPAD_LEFT) == KEY_REPEAT)
			HandleUINavigation(firstElement, lastElement, MenuNavDirection::LEFT);
		else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT || Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_DPAD_RIGHT) == KEY_REPEAT)
			HandleUINavigation(firstElement, lastElement, MenuNavDirection::RIGHT);
	}

	if (!Engine::GetInstance().paused)
	{
		if (player && player->active)
		{
			SDL_Texture* currentHabilityTex = nullptr;

			int playerFullHearts = player->playerCurrentHp / 2;
			int damagedHearts = 5 - playerFullHearts;
			int halfHeart = player->playerCurrentHp % 2;

			Engine::GetInstance().render->DrawTexture(heartContainerTexture, 200, 64, NULL, 0.0f);

			for (int i = 0; i < playerFullHearts; ++i) {
				Engine::GetInstance().render->DrawTexture(heartFullTexture, 230 + (i * 70), 74, NULL, 0.0f);
			}

			if (halfHeart == 1) {
				Engine::GetInstance().render->DrawTexture(heartHalfTexture, 230 + (playerFullHearts * 70), 74, NULL, 0.0f);
			}
			else if (damagedHearts > 0){
				Engine::GetInstance().render->DrawTexture(heartEmptyTexture, 230 + (playerFullHearts * 70), 74, NULL, 0.0f);
			}

			for (int i = playerFullHearts + 1; i < 5; ++i) {
				Engine::GetInstance().render->DrawTexture(heartEmptyTexture, 230 + (i * 70), 74, NULL, 0.0f);
			}

			if (player->hasPowerJump == true) {
				currentHabilityTex = habilityPowerJumpTexture;
			}
			else {
				currentHabilityTex = habilityContainerTexture;
			}
			Engine::GetInstance().render->DrawTexture(currentHabilityTex, 0, 0, NULL, 0.0f);
		}
	}

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL2);
	}
}

void Scene::UnloadLevel1() {

	// Clean up UI elements related to the Level1
	auto& uiManager = Engine::GetInstance().uiManager;
	uiManager->CleanUp();

	// Reset player reference (sets the shared_ptr to nullptr)
	player.reset();

	// Clean up map and entities
	Engine::GetInstance().map->CleanUp();
	Engine::GetInstance().entityManager->CleanUp();

	destroyedEntitiesIds.clear();

}

void  Scene::PostUpdateLevel1() {
	//L15 TODO 3: Call the function to load entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) 
	{
		Engine::GetInstance().entityManager->ClearNonPlayerEntities();

		Engine::GetInstance().map->LoadEntities(player);

		LoadGame();
	}

	//L15 TODO 4: Call the function to save entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) 
	{
		SaveGame();
	}
}

// *********************************************
// Level 2 functions
// *********************************************

void Scene::LoadLevel2() {

	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/that-8-bit-music-322062.wav");

	//Call the function to load the map.
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplateLevel2.tmx");

	//Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player);

	LoadGame();
}

void Scene::UpdateLevel2(float dt) {

	HandlePause();

	if (Engine::GetInstance().paused) {
		int w, h;
		Engine::GetInstance().window->GetWindowSize(w, h);
		SDL_Rect fullscreenRect = { 0, 0, w, h };

		Engine::GetInstance().render->DrawRectangle(fullscreenRect, 0, 0, 0, 150, true, false);
	}

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		ChangeScene(SceneID::LEVEL1);
	}
}

void Scene::UnloadLevel2() {

	// Clean up UI elements related to the Level2
	auto& uiManager = Engine::GetInstance().uiManager;
	uiManager->CleanUp();

	// Reset player reference (sets the shared_ptr to nullptr)
	player.reset();

	// Clean up map and entities
	Engine::GetInstance().map->CleanUp();
	Engine::GetInstance().entityManager->CleanUp();

	destroyedEntitiesIds.clear();
}

void Scene::ActivateGameOver()
{
	ShowDeathScreen();
}

void Scene::ShowDeathScreen()
{
	if (isGameOver)	
		return;

	isGameOver = true;
	Engine::GetInstance().paused = true;

	int w, h;
	Engine::GetInstance().window->GetWindowSize(w, h);

	int buttonWidth = 290;
	int buttonHeight = 86;

	SDL_Rect tryAgainButtonRect = { ((w / 2) - buttonWidth) - 20 ,(h / 2) + 40 , buttonWidth, buttonHeight };
	SDL_Rect goToMenuButtonRect = { ((w / 2) + 10) + 20 , (h / 2) + 40 , buttonWidth, buttonHeight };

	auto tryAgainButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 10, " TRY AGAIN ", tryAgainButtonRect, this);
	tryAgainButton->SetTexture(gameOverTryAgainButtonTexture);

	auto goToMenuButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 11, " GO TO MENU ", goToMenuButtonRect, this);
	goToMenuButton->SetTexture(gameOverGoToMenuButtonTexture);
}

// *********************************************
// Video rendering functions
// *********************************************

void Scene::OnVideoFrame(plm_t* mpeg, plm_frame_t* frame, void* user)
{
	VideoData* vd = static_cast<VideoData*>(user);

	if (vd->buffer) 
	{
	
		plm_frame_to_rgba(frame, vd->buffer, vd->width * 4);
	}
}

void Scene::PlayIntroVideo() {

	plm = plm_create_with_filename("Assets/Video/AnimaticaFinal.mpg");

	if (!plm) {
		LOG("Error: No se pudo cargar el video AnimaticaFinal.mpg");
		ChangeScene(SceneID::LEVEL1); // Fallback: if failed to load video, goes directly to level 1
		return;
	}

	plm_set_loop(plm, 0); // 0 = No loop, 1 = Loop indefinitely
	introVideo.width = plm_get_width(plm);
	introVideo.height = plm_get_height(plm);

	introVideo.buffer = new uint8_t[introVideo.width * introVideo.height * 4];

	// We create an SDL texture prepared to receive pixels by streaming
	introVideo.texture = SDL_CreateTexture(
		Engine::GetInstance().render->renderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		introVideo.width,
		introVideo.height
	);

	plm_set_video_decode_callback(plm, OnVideoFrame, &introVideo);

	isPlayingVideo = true;
	Engine::GetInstance().uiManager->CleanUp();
}

void Scene::StopIntroVideo() {
	isPlayingVideo = false;

	if (plm) plm_destroy(plm);
	if (introVideo.texture) SDL_DestroyTexture(introVideo.texture);
	if (introVideo.buffer) delete[] introVideo.buffer;

	plm = nullptr;
	introVideo.texture = nullptr;
	introVideo.buffer = nullptr;
}

// *********************************************
// Saving/Loading functions
// *********************************************

void Scene::SaveGame() 
{
	//LOG("Saving Game...");
	pugi::xml_document saveDoc;

	saveDoc.load_file("savegame.xml");

	pugi::xml_node root = saveDoc.child("save_estate");
	if (!root) {
		root = saveDoc.append_child("save_estate");
	}

	pugi::xml_node playerNode = root.child("player");
	if (!playerNode) {
		playerNode = root.append_child("player");
	}

	playerNode.remove_children();

	if (player != nullptr)
	{
		pugi::xml_node posNode = playerNode.append_child("position");
		posNode.append_attribute("x").set_value(player->GetPosition().getX());
		posNode.append_attribute("y").set_value(player->GetPosition().getY());

		pugi::xml_node statsNode = playerNode.append_child("stats");
		statsNode.append_attribute("currentHp").set_value(player->playerCurrentHp);
		statsNode.append_attribute("maxHp").set_value(player->playerMaxHp);

		pugi::xml_node upgradesNode = playerNode.append_child("upgrades");	
		upgradesNode.append_attribute("hasPowerJump").set_value(player->hasPowerJump);
	}

	pugi::xml_node worldNode = root.child("world");
	if (!worldNode) worldNode = root.append_child("world");

	std::string currentMapName = Engine::GetInstance().map->mapFileName;

	pugi::xml_node levelNode = worldNode.find_child_by_attribute("level", "name", currentMapName.c_str());
	if (!levelNode)
	{
		levelNode = worldNode.append_child("level");
		levelNode.append_attribute("name").set_value(currentMapName.c_str());
	}

	levelNode.remove_children();
	pugi::xml_node destroyedNode = levelNode.append_child("entities");
	
	for (int id : destroyedEntitiesIds)
	{
		pugi::xml_node entNode = destroyedNode.append_child("entity");
		entNode.append_attribute("id").set_value(id);
	}

	pugi::xml_node checkpointNode = levelNode.append_child("active_checkpoint");
	if (checkpointNode)
	{
		levelNode.remove_child(checkpointNode);
	}
	checkpointNode = levelNode.append_child("active_checkpoint");

	int activeCpId = -1;
	for (Checkpoint* cp : Checkpoint::allCheckpoints)
	{
		if (cp->IsActive()) 
		{
			activeCpId = cp->tiledId;
			break;
		}
	}

	checkpointNode.append_attribute("id").set_value(activeCpId);	

	saveDoc.save_file("Saves/savegame.xml");
	//LOG("Game successfully saved in Saves/savegame.xml");
}

void Scene::LoadGame() 
{
	//LOG("Loading Game...");
	pugi::xml_document loadDoc;
	pugi::xml_parse_result result = loadDoc.load_file("Saves/savegame.xml");

	if (!result) {
		LOG("There is no save or error reading Saves/savegame.xml");
		return;
	}

	pugi::xml_node root = loadDoc.child("save_estate");
	
	pugi::xml_node playerNode = root.child("player");
	if (playerNode && player != nullptr) 
	{
		pugi::xml_node posNode = playerNode.child("position");
		if (posNode) 
		{
			float x = posNode.attribute("x").as_float();
			float y = posNode.attribute("y").as_float();
			player->SetPosition(Vector2D(x, y));
		}

		pugi::xml_node statsNode = playerNode.child("stats");
		if (statsNode) 
		{
			player->playerCurrentHp = statsNode.attribute("currentHp").as_int();
			player->playerMaxHp = statsNode.attribute("maxHp").as_int();
		}

		pugi::xml_node upgradesNode = playerNode.child("upgrades");
		if (upgradesNode) 
		{
			player->hasPowerJump = upgradesNode.attribute("hasPowerJump").as_bool();
		}
	}

	std::string currentMapName = Engine::GetInstance().map->mapFileName;
	pugi::xml_node levelNode = root.child("world").find_child_by_attribute("level", "name", currentMapName.c_str());

	if (levelNode) 
	{		
		destroyedEntitiesIds.clear();

		pugi::xml_node destroyedNode = levelNode.child("entities");
		for (pugi::xml_node entNode = destroyedNode.child("entity"); entNode; entNode = entNode.next_sibling("entity")) 
		{
			int deadId = entNode.attribute("id").as_int();

			destroyedEntitiesIds.push_back(deadId);

			std::shared_ptr<Entity> entityToKill = Engine::GetInstance().entityManager->GetEntityByTiledId(deadId);
			if (entityToKill != nullptr)
			{
				entityToKill->Destroy();
			}
		}	

		pugi::xml_node checkpointNode = levelNode.child("active_checkpoint");
		if (checkpointNode)
		{
			int activeCpId = checkpointNode.attribute("id").as_int(-1);
			if (activeCpId != -1)
			{
				std::shared_ptr<Entity> cpEntity = Engine::GetInstance().entityManager->GetEntityByTiledId(activeCpId);

				if (cpEntity != nullptr && cpEntity->type == EntityType::CHECKPOINT)
				{
					std::shared_ptr<Checkpoint> cp = std::static_pointer_cast<Checkpoint>(cpEntity);
					cp->SetActive(true);
				}
			}
		}
	}
	//LOG("Game successfully loaded from savegame.xml");
}

// *********************************************
// UI Navigation
// *********************************************

void Scene::HandleUINavigation(int initialID, int finalID, MenuNavDirection direction) {
	/* MENU NAVIGATION WITH KEYS */

	auto selectedUI = Engine::GetInstance().uiManager->GetElement(selectedUIID);

	switch (direction) {
		case MenuNavDirection::UP:
		{
			selectedUI->isSelected = false;
			selectedUI->state = UIElementState::NORMAL;

			if (selectedUIID == initialID)
				selectedUIID = finalID;
			else
				selectedUIID--;

			Engine::GetInstance().uiManager->GetElement(selectedUIID)->isSelected = true;
			break;
		}
		case MenuNavDirection::DOWN:
		{
			selectedUI->isSelected = false;
			selectedUI->state = UIElementState::NORMAL;

			if (selectedUIID == finalID)
				selectedUIID = initialID;
			else
				selectedUIID++;

			Engine::GetInstance().uiManager->GetElement(selectedUIID)->isSelected = true;
			break;
		}
		case MenuNavDirection::LEFT:
		{
			if (selectedUI->type == UIElementType::SLIDER) {
				auto sliderElement = std::static_pointer_cast<UISlider>(selectedUI);
				sliderElement->SetValue(sliderElement->GetValue() - 1);
			}

			break;
		}
		case MenuNavDirection::RIGHT:
		{
			if (selectedUI->type == UIElementType::SLIDER) {
				auto sliderElement = std::static_pointer_cast<UISlider>(selectedUI);
				sliderElement->SetValue(sliderElement->GetValue() + 1);
			}
			break;
		}
	}
}
