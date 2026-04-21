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
	if (player) return player->GetPosition();
	else Vector2D(0,0);
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

	/*if (mainMenuBackground == nullptr)
	{
		mainMenuBackground = Engine::GetInstance().textures->Load("Assets/Textures/UI/fondo_menu (con titulo).png");
	}*/	

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

	playButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/PlayButton.png");
	optionsButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/OptionsButton.png");
	exitButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/ExitButton.png");

	// Instantiate a UIButton in the Scene
	SDL_Rect playButtonRect = { 146 , 412 , buttonWidth, buttonHeight };
	SDL_Rect optionsButtonRect = { 149 , 578 , buttonWidth, buttonHeight };
	SDL_Rect exitButtonRect = { 148 , 759 , buttonWidth, buttonHeight };

	auto playButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, " ",playButtonRect, this);
	playButton->SetTexture(playButtonTexture);

	auto optionsButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, " ", optionsButtonRect, this);
	optionsButton->SetTexture(optionsButtonTexture);

	auto exitButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 3, " ", exitButtonRect, this);
	exitButton->SetTexture(exitButtonTexture);

	//std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, "PLAY", playButtonRect, this));
	//std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, "OPTIONS", optionsButtonRect, this));
	//std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 3, "EXIT", exitButtonRect, this));
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

	if (mainMenuBackground != nullptr)
	{
		Engine::GetInstance().render->DrawTexture(mainMenuBackground, 0, 0, NULL, 0.0f);
	}
	if (currentMenuState == MainMenuState::OPTIONS) {
		int w, h;
		Engine::GetInstance().window->GetWindowSize(w, h);

		SDL_Rect fullscreenRect = { 0, 0, w, h };

		Engine::GetInstance().render->DrawRectangle(fullscreenRect, 0, 0, 0, 150, true, false);

		sliderBoxTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderBox.png");
		Engine::GetInstance().render->DrawTexture(sliderBoxTexture, (w - sliderBoxTexture->w) / 2, (h - (sliderBoxTexture->h * 2)) / 2, NULL, 0.0f);

		sliderAudioTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/AudioIcon.png");
		Engine::GetInstance().render->DrawTexture(sliderAudioTexture, ((w - sliderAudioTexture->w) / 2) - 200, ((h - sliderAudioTexture->h) / 2) - 65, NULL, 0.0f);

	}

}

void Scene::HandleMainMenuUIEvents(UIElement* uiElement)
{
	switch (uiElement->id)
	{
	case 1: // Button MyButton
		LOG("Main Menu: MyButton clicked!");
		ChangeScene(SceneID::LEVEL1);
		break;
	case 2: // Button Options
		LOG("Main Menu: Options button clicked!");
		LoadOptionsMainMenu();
		break;
	case 3: // Button Exit
		LOG("Main Menu: Exit clicked!");
		Engine::GetInstance().quit = true;
		break;
	case 4:
	{
		UISlider * slider = static_cast<UISlider*>(uiElement);
		float volume = slider->GetValue() / 100.0f;
		Engine::GetInstance().audio->SetMusicVolume(volume);
		break;
	}
	case 5:
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

	mainMenuBackground = Engine::GetInstance().textures->Load("Assets/Textures/UI/menu INCORPUS.png");

	int screenWidth, screenHeight;
	Engine::GetInstance().window->GetWindowSize(screenWidth, screenHeight);

	SDL_Texture* sliderBarTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderBar.png");
	SDL_Texture* sliderKnobTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderKnob.png");

	SDL_Rect sliderBounds = { ((screenWidth - sliderBarTexture->w) / 2) + 50, (screenHeight / 2) - 80, 399, 25 };
	//Engine::GetInstance().uiManager->CreateUIElement(UIElementType::SLIDER, 4, "VOLUME", sliderBounds, this);
	auto audioSliderElement = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::SLIDER, 4, "VOLUME", sliderBounds, this);

	auto audioSlider = std::static_pointer_cast<UISlider>(audioSliderElement);
	audioSlider->SetTexture(sliderBarTexture);
	audioSlider->SetKnobTexture(sliderKnobTexture);

	backButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/BackButton.png");

	SDL_Rect backButtonRect = { (screenWidth - backButtonTexture->w) / 2, 736, 290, 86};

	auto backButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 5, " ", backButtonRect, this);
	backButton->SetTexture(backButtonTexture);

	//Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 5, "BACK", backButtonPos, this);
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
	case 6: // PAUSE MENU OPTIONS: Volume Slider
	{
		UISlider* slider = static_cast<UISlider*>(uiElement);
		float volume = slider->GetValue() / 100.0f;
		Engine::GetInstance().audio->SetMusicVolume(volume);
		break;
	}
	case 7: // PAUSE MENU OPTIONS: Back Button
	{
		Engine::GetInstance().uiManager->CleanUp();
		LoadPauseMenu();
		break;
	}	
	}
}

void Scene::HandlePause() {
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		Engine::GetInstance().Func_PauseEngine();

		if (Engine::GetInstance().paused) {
			LoadPauseMenu();
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

	auto optionsButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, " OPTIONS ", optionsButtonRect, this);
	optionsButton->SetTexture(optionsPauseButtonTexture);

	auto quitToMenuButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 3, " QUIT TO MENU ", quitToMenuButtonRect, this);
	quitToMenuButton->SetTexture(menuQuitPauseButtonTexture);

	auto quitToDesktopButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 4, " QUIT TO DESKTOP ", quitToDesktopButtonRect, this);
	quitToDesktopButton->SetTexture(gameQuitButtonTexture);

	//std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, "Continue", continueButtonRect, this));
	//std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, "Options", optionsButtonRect, this));
	//std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 3, "Quit", quitToMenuButtonRect, this));
	///std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 20, "Salir", quitToDesktopButtonRect, this));
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
	std::shared_ptr<UIElement> elem = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::CHECKBOX, 5, " FULLSCREEN ", checkBoxPos, this);

	SDL_Texture* sliderBarTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderBar.png");
	SDL_Texture* sliderKnobTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderKnob.png");

	SDL_Rect sliderBounds = { center_window_posX, center_window_posY - 80, 399, 25 };
	//Engine::GetInstance().uiManager->CreateUIElement(UIElementType::SLIDER, 6, " VOLUME ", sliderBounds, this);
	auto audioSliderElement = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::SLIDER, 6, " VOLUME ", sliderBounds, this);

	auto audioSlider = std::static_pointer_cast<UISlider>(audioSliderElement);
	audioSlider->SetTexture(sliderBarTexture);
	audioSlider->SetKnobTexture(sliderKnobTexture);

	backButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/BackButton.png");

	SDL_Rect backButtonPos = { center_window_posX, center_window_posY + 140, 290, 86};

	auto backButton = Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 7, " ", backButtonPos, this);
	backButton->SetTexture(backButtonTexture);
	//Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 7, " BACK ", backButtonPos, this);
}

/*
void Scene::LoadPauseQuitMenu() {

	currentPauseState = PauseMenuState::QUIT_CONFIRM;

	int buttonWidth = 120;
	int buttonHeight = 20;
	int buttonMargin = 10;

	int centerWindowPosX = (Engine::GetInstance().window->width / 2) - buttonWidth / 2;
	int centerWindowPosY = Engine::GetInstance().window->height / 2;

	SDL_Rect menuButtonPos = { centerWindowPosX, centerWindowPosY - buttonHeight - buttonMargin, buttonWidth, buttonHeight };
	SDL_Rect desktopButtonPos = { centerWindowPosX, centerWindowPosY, buttonWidth, buttonHeight };
	SDL_Rect cancelButtonPos = { centerWindowPosX, centerWindowPosY + buttonHeight*2 + buttonMargin, buttonWidth, buttonHeight };

	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 4, "Quit to Main Menu", menuButtonPos, this));
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 5, "Quit to Desktop", desktopButtonPos, this));
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 6, "Cancel", cancelButtonPos, this));

}
*/

// *********************************************
// Level 1 functions
// *********************************************

void Scene::LoadLevel1() {

	//Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/level-iv-339695.wav");

	//Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");

	//Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player);

	//Create a new item using the entity manager and set the position to (200, 672) to test
	std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
	item->position = Vector2D(200, 672);
	item->Start(); //L17 Important call Start

	//Create a new enemy 
	std::shared_ptr<Enemy> enemy1 = std::dynamic_pointer_cast<Enemy>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
	enemy1->position = Vector2D(384, 672);
	enemy1->Start(); //L17 Important call Start

	//Create a new enemy 
	std::shared_ptr<Enemy> enemy2 = std::dynamic_pointer_cast<Enemy>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY));
	enemy2->position = Vector2D(1380, 672);
	enemy2->Start(); //L17 Important call Start
}

void Scene::UpdateLevel1(float dt) {
	HandlePause();

	if (Engine::GetInstance().paused) {
		int w, h;
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
			sliderBoxTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderBox.png");
			Engine::GetInstance().render->DrawTexture(sliderBoxTexture, (w - sliderBoxTexture->w) / 2, (h - (sliderBoxTexture->h * 2)) / 2, NULL, 0.0f);

			sliderAudioTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/AudioIcon.png");
			Engine::GetInstance().render->DrawTexture(sliderAudioTexture, ((w - sliderAudioTexture->w) / 2) - 200, ((h - sliderAudioTexture->h) / 2) - 65, NULL, 0.0f);
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

}

void  Scene::PostUpdateLevel1() {

	//L15 TODO 3: Call the function to load entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		Engine::GetInstance().map->LoadEntities(player);
	}

	//L15 TODO 4: Call the function to save entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		Engine::GetInstance().map->SaveEntities(player);
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

}