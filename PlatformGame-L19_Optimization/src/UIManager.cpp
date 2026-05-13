#include "UIManager.h"
#include "UIButton.h"
#include "UISlider.h"
#include "UICheckBox.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "EntityManager.h"

UIManager::UIManager() :Module()
{
	name = "UIManager";
}

UIManager::~UIManager() {}

bool UIManager::Awake()
{
	LOG("Loading UI");
	//LoadUITextures();
	bool ret = true;

	return ret;
}

bool UIManager::Start()
{
	

	
	return true;
}

std::shared_ptr<UIElement> UIManager::CreateUIElement(UIElementType type, int id, const char* text, SDL_Rect bounds, Module* observer, SDL_Rect sliderBounds)
{
	std::shared_ptr<UIElement> uiElement = std::make_shared<UIElement>();
	uiElement->type = type;
	// L16: TODO 1: Implement CreateUIElement function that instantiates a new UIElement according to the UIElementType and add it to the list of UIElements
	//Call the constructor according to the UIElementType
	switch (type)
	{
	case UIElementType::BUTTON:
		uiElement = std::make_shared<UIButton>(id, bounds, text);
		break;
	case UIElementType::SLIDER:
		uiElement = std::make_shared<UISlider>(id, bounds, text);
		break;
	case UIElementType::CHECKBOX: {
		uiElement = std::make_shared<UICheckBox>(id, bounds, text);
		if (text == " FULLSCREEN ") {
			bool value = Engine::GetInstance().render->IsFullScreen();
			uiElement = std::make_shared<UICheckBox>(id, bounds, text, value);
		}
		else {
			uiElement = std::make_shared<UICheckBox>(id, bounds, text);
		}
		break;
	}
	}
	
	//Set the observer
	uiElement->observer = observer;

	// Created GuiControls are add it to the list of controls
	UIElementsList.push_back(uiElement);

	return uiElement;
}

bool UIManager::Update(float dt)
{	
	std::vector<std::shared_ptr<UIElement>> elementsToUpdate;
	for (const auto& uiElement : UIElementsList)
	{
		elementsToUpdate.push_back(uiElement);
	}

	// 2. Actualizamos solo los elementos de nuestra copia segura
	for (const auto& uiElement : elementsToUpdate)
	{
		if (!uiElement->pendingToDelete)
		{
			uiElement->Update(dt);
		}
	}
	//List to store entities pending deletion
	std::list<std::shared_ptr<UIElement>> pendingDelete;

	for (const auto& uiElement : UIElementsList)
	{
		//If the entity is marked for deletion, add it to the pendingDelete list
		if (uiElement->pendingToDelete)
		{
			pendingDelete.push_back(uiElement);
		}
	}

	//Now iterates over the pendingDelete list and destroys the uiElement
	for (const auto uiElement : pendingDelete)
	{
		uiElement->CleanUp();
		UIElementsList.remove(uiElement);
	}

	return true;
}

bool UIManager::CleanUp()
{
	for (const auto& uiElement : UIElementsList)
	{
		uiElement->CleanUp();
	}

	UIElementsList.clear();

	return true;
}

std::shared_ptr<UIElement> UIManager::GetElement(int id) {
	for (const auto& uiElement : UIElementsList)
	{
		if (uiElement->id == id)
			return uiElement;
	}
}

// *********************************************
// Load Textures
// *********************************************
void UIManager::LoadUITextures() {
	
	/* Main Menu UI*/
	mainMenuBackground = Engine::GetInstance().textures->Load("Assets/Textures/UI/menu INCORPUS.png");
	sliderBarTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderBar.png");
	sliderKnobTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderKnob.png");
	backButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/BackButton.png");
	playButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/PlayButton.png");
	newgameButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/NewGameButton.png");
	optionsButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/OptionsButton.png");
	exitButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/ExitButton.png");
	sliderBoxTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderBox.png");
	sliderAudioTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/AudioIcon.png");
	
	
	/* Pause UI*/
	pauseOptionsMenuTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Fondo_pause_menu.png");
	continuePauseButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/ContinueButton.png");
	optionsPauseButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/OptionsButton.png");
	menuQuitPauseButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/QuitToMenuButton.png");
	gameQuitButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/QuitGameButton.png");
	sliderBoxTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderBox.png");
	sliderAudioTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/AudioIcon.png");

	/*Player UI*/
	heartFullTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Corazon_full.png");
	heartHalfTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Corazon_meitat.png");
	heartEmptyTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Corazon_muerto.png");
	heartContainerTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Caja_vida.png");
	habilityContainerTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Hueco_habilidades_vacio.png");
	habilityPowerJumpTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Hueco_habilidades_Salto.png");

	/* GameOver UI*/
	gameOverTryAgainButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/DeathMenu/TryAgainButton.png");
	gameOverGoToMenuButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/DeathMenu/GoToMenuButton.png");
	deathScreenMenuTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/DeathMenu/Fondo_death_menu.png");
}

// *********************************************
// Main Menu
// *********************************************
void UIManager::LoadMainMenuButtons() {
	CleanUp();

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

	auto playButton = CreateUIElement(UIElementType::BUTTON, 1, " ", playButtonRect, Engine::GetInstance().scene->GetScene());
	playButton->SetTexture(playButtonTexture);
	playButton->isSelected = true;
	selectedUIID = 1;

	auto optionsButton = CreateUIElement(UIElementType::BUTTON, 2, " ", optionsButtonRect, Engine::GetInstance().scene->GetScene());
	optionsButton->SetTexture(optionsButtonTexture);

	auto exitButton = CreateUIElement(UIElementType::BUTTON, 3, " ", exitButtonRect, Engine::GetInstance().scene->GetScene());
	exitButton->SetTexture(exitButtonTexture);
}

void UIManager::ShowMainMenuButtons()
{
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
	else if (currentMenuState == MainMenuState::PLAY_MENU)
	{
		int w, h;

		Engine::GetInstance().window->GetWindowSize(w, h);

		SDL_Rect fullscreenRect = { 0, 0, w, h };

		Engine::GetInstance().render->DrawRectangle(fullscreenRect, 0, 0, 0, 150, true, false);

		firstElement = 7;
		lastElement = 9;
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

void UIManager::LoadOptionsMainMenu()
{

	CleanUp();

	currentMenuState = MainMenuState::OPTIONS;

	int screenWidth, screenHeight;
	Engine::GetInstance().window->GetWindowSize(screenWidth, screenHeight);

	SDL_Rect checkBoxPos = { ((screenWidth - sliderBarTexture->w) / 2) + 50, (screenHeight / 2) - 200, 40, 40 };
	auto fullcreenElement = CreateUIElement(UIElementType::CHECKBOX, 4, " FULLSCREEN ", checkBoxPos, Engine::GetInstance().scene->GetScene());
	fullcreenElement->isSelected = true;
	selectedUIID = 4;

	SDL_Rect sliderBounds = { ((screenWidth - sliderBarTexture->w) / 2) + 50, (screenHeight / 2) - 80, 399, 25 };
	auto musicSliderElement = CreateUIElement(UIElementType::SLIDER, 5, "MUSIC", sliderBounds, Engine::GetInstance().scene->GetScene());

	SDL_Rect fxSliderBounds = { ((screenWidth - sliderBarTexture->w) / 2) + 50, (screenHeight / 2) + 90, 399, 25 };
	auto fxSliderElement = CreateUIElement(UIElementType::SLIDER, 6, "FX", fxSliderBounds, Engine::GetInstance().scene->GetScene());

	auto musicSlider = std::static_pointer_cast<UISlider>(musicSliderElement);
	musicSlider->SetTexture(sliderBarTexture);
	musicSlider->SetKnobTexture(sliderKnobTexture);

	auto fxSlider = std::static_pointer_cast<UISlider>(fxSliderElement);
	fxSlider->SetTexture(sliderBarTexture);
	fxSlider->SetKnobTexture(sliderKnobTexture);

	SDL_Rect backButtonRect = { (screenWidth - backButtonTexture->w) / 2, 736, 290, 86 };

	auto backButton = CreateUIElement(UIElementType::BUTTON, 7, " ", backButtonRect, Engine::GetInstance().scene->GetScene());
	backButton->SetTexture(backButtonTexture);
}
void UIManager::LoadPlayMainMenu()
{
	CleanUp();

	currentMenuState = MainMenuState::PLAY_MENU;

	int buttonWidth = 290;
	int buttonHeight = 86;
	int buttonMargin = 20;

	int screenWidth, screenHeight;
	Engine::GetInstance().window->GetWindowSize(screenWidth, screenHeight);

	SDL_Rect newGameButtonRect = { (screenWidth - buttonWidth) / 2, (screenHeight / 2) - buttonHeight - buttonMargin, buttonWidth, buttonHeight };
	SDL_Rect continueButtonRect = { (screenWidth - buttonWidth) / 2, (screenHeight / 2) + buttonMargin, buttonWidth, buttonHeight };
	SDL_Rect backButtonRect = { (screenWidth - buttonWidth) / 2, (screenHeight / 2) + buttonHeight + (buttonMargin * 3), buttonWidth, buttonHeight };

	auto newGameButton = CreateUIElement(UIElementType::BUTTON, 8, " ", newGameButtonRect, Engine::GetInstance().scene->GetScene());
	newGameButton->SetTexture(newgameButtonTexture);
	newGameButton->isSelected = true;
	selectedUIID = 8;

	auto continueButton = CreateUIElement(UIElementType::BUTTON, 9, " ", continueButtonRect, Engine::GetInstance().scene->GetScene());
	continueButton->SetTexture(continuePauseButtonTexture);

	auto backButton = CreateUIElement(UIElementType::BUTTON, 7, " ", backButtonRect, Engine::GetInstance().scene->GetScene());
	backButton->SetTexture(backButtonTexture);
}

void UIManager::HandleMainMenuUIEvents(UIElement* uiElement)
{
	switch (uiElement->id)
	{
	case 1: // Play Button
		LoadPlayMainMenu();
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
		UISlider* slider = static_cast<UISlider*>(uiElement);
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
	{
		LOG("Main Menu: Back button clicked!");
		LoadMainMenuButtons();
		break;
	}
	case 8: // NEW GAME BUTTON
	{
		LOG("STARTING NEW GAME");
		std::remove("Saves/savegame.xml");

		Engine::GetInstance().scene->ChangeScene(SceneID::LEVEL);
		//Engine::GetInstance().scene->PlayIntroVideo();
		break;
	}
	case 9: // CONTINUE BUTTON
	{
		LOG("CONTINUING GAME");
		Engine::GetInstance().scene->ChangeScene(SceneID::LEVEL);
		Engine::GetInstance().scene->LoadGame();
		break;
	}
	default:
		break;
	}
}

// *********************************************
// Pause Menu
// *********************************************
void UIManager::LoadPauseMenu() {
	CleanUp();
	currentPauseState = PauseMenuState::MAIN;

	int button_width = 290;
	int button_height = 86;
	int button_margin = 20;

	int center_window_posX = (Engine::GetInstance().window->width / 2) - button_width / 2;
	int center_window_posY = Engine::GetInstance().window->height / 2;

	SDL_Rect continueButtonRect = { center_window_posX, center_window_posY - (button_height * 2) - (button_margin * 2), button_width, button_height };
	SDL_Rect optionsButtonRect = { center_window_posX, continueButtonRect.y + button_height + button_margin, button_width, button_height };
	SDL_Rect quitToMenuButtonRect = { center_window_posX, optionsButtonRect.y + button_height + button_margin, button_width, button_height };
	SDL_Rect quitToDesktopButtonRect = { center_window_posX, quitToMenuButtonRect.y + button_height + button_margin, button_width, button_height };

	auto continueButton = CreateUIElement(UIElementType::BUTTON, 1, " CONTINUE ", continueButtonRect, Engine::GetInstance().scene->GetScene());
	continueButton->SetTexture(continuePauseButtonTexture);
	continueButton->isSelected = true;
	selectedUIID = 1;

	auto optionsButton = CreateUIElement(UIElementType::BUTTON, 2, " OPTIONS ", optionsButtonRect, Engine::GetInstance().scene->GetScene());
	optionsButton->SetTexture(optionsPauseButtonTexture);

	auto quitToMenuButton = CreateUIElement(UIElementType::BUTTON, 3, " QUIT TO MENU ", quitToMenuButtonRect, Engine::GetInstance().scene->GetScene());
	quitToMenuButton->SetTexture(menuQuitPauseButtonTexture);

	auto quitToDesktopButton = CreateUIElement(UIElementType::BUTTON, 4, " QUIT TO DESKTOP ", quitToDesktopButtonRect, Engine::GetInstance().scene->GetScene());
	quitToDesktopButton->SetTexture(gameQuitButtonTexture);
}

void UIManager::LoadPauseOptionsMenu()
{
	CleanUp();
	currentPauseState = PauseMenuState::OPTIONS;

	int button_width = 290;
	int button_height = 86;
	int button_margin = 20;

	int center_window_posX = (Engine::GetInstance().window->width / 2) - button_width / 2;
	int center_window_posY = Engine::GetInstance().window->height / 2;

	SDL_Rect checkBoxPos = { center_window_posX - 130, center_window_posY - 200, 40, 40 };
	auto fullcreenElement = CreateUIElement(UIElementType::CHECKBOX, 5, " FULLSCREEN ", checkBoxPos, Engine::GetInstance().scene->GetScene());
	fullcreenElement->isSelected = true;
	selectedUIID = 5;

	SDL_Texture* sliderBarTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderBar.png");
	SDL_Texture* sliderKnobTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderKnob.png");

	SDL_Rect musicSliderBounds = { center_window_posX, center_window_posY - 80, 399, 25 };
	auto musicSliderElement = CreateUIElement(UIElementType::SLIDER, 6, " MUSIC ", musicSliderBounds, Engine::GetInstance().scene->GetScene());

	SDL_Rect sfxSliderBounds = { center_window_posX, center_window_posY + 75, 399, 25 };
	auto sfxSliderElement = CreateUIElement(UIElementType::SLIDER, 7, " SFX ", sfxSliderBounds, Engine::GetInstance().scene->GetScene());

	auto musicSlider = std::static_pointer_cast<UISlider>(musicSliderElement);
	musicSlider->SetTexture(sliderBarTexture);
	musicSlider->SetKnobTexture(sliderKnobTexture);

	auto sfxSlider = std::static_pointer_cast<UISlider>(sfxSliderElement);
	sfxSlider->SetTexture(sliderBarTexture);
	sfxSlider->SetKnobTexture(sliderKnobTexture);

	backButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/BackButton.png");

	SDL_Rect backButtonPos = { center_window_posX, center_window_posY + 175, 290, 86 };

	auto backButton = CreateUIElement(UIElementType::BUTTON, 8, " ", backButtonPos, Engine::GetInstance().scene->GetScene());
	backButton->SetTexture(backButtonTexture);
}

void UIManager::ShowPauseMenu() {
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

	if (Engine::GetInstance().scene->isGameOver)
	{
		Engine::GetInstance().render->DrawRectangle(fullscreenRect, 0, 0, 0, 150, true, false);
		Engine::GetInstance().render->DrawTexture(deathScreenMenuTexture, (w - deathScreenMenuTexture->w) / 2, (h - deathScreenMenuTexture->h) / 2, NULL, 0.0f);
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

void UIManager::HandleUINavigation(int initialID, int finalID, MenuNavDirection direction) {
	/* MENU NAVIGATION WITH KEYS */

	auto selectedUI = GetElement(selectedUIID);

	switch (direction) {
	case MenuNavDirection::UP:
	{
		selectedUI->isSelected = false;
		selectedUI->state = UIElementState::NORMAL;

		if (selectedUIID == initialID)
			selectedUIID = finalID;
		else
			selectedUIID--;

		GetElement(selectedUIID)->isSelected = true;
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

		GetElement(selectedUIID)->isSelected = true;
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

void UIManager::HandlePauseMenuUIEvents(UIElement* uiElement)
{
	switch (uiElement->id)
	{
	case 1: // PAUSE MENU: CONTINUE
		LOG("PAUSE MENU: CONTINUE clicked!");
		Engine::GetInstance().Func_PauseEngine();
		CleanUp();
		break;
	case 2: // PAUSE MENU: OPTIONS
		LoadPauseOptionsMenu();
		break;
	case 3: // PAUSE MENU: QUIT TO MENU
		CleanUp();
		Engine::GetInstance().entityManager->CleanUp(true);
		/*Engine::GetInstance().entityManager->ClearNonPlayerEntities();

		if (player != nullptr)
		{
			Engine::GetInstance().entityManager->DestroyEntity(player);
			player.reset();
		}*/

		Engine::GetInstance().scene->ChangeScene(SceneID::MAIN_MENU);
		Engine::GetInstance().Func_PauseEngine();
		break;
	case 4: // PAUSE MENU: QUIT GAME
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
		CleanUp();
		LoadPauseMenu();
		break;
	}
	case 10: //	DEATH SCREEN: Try Again Button
	{
		Engine::GetInstance().scene->isGameOver = false;
		Engine::GetInstance().paused = false;
		CleanUp();
		Engine::GetInstance().scene->ChangeScene(Engine::GetInstance().scene->GetCurrentScene());
		break;
	}
	case 11: //	DEATH SCREEN: Go To Menu Button
	{
		Engine::GetInstance().scene->isGameOver = false;
		Engine::GetInstance().paused = false;
		CleanUp();
		Engine::GetInstance().scene->ChangeScene(SceneID::MAIN_MENU);
		break;
	}
	}
}


// *********************************************
// Player UI
// *********************************************
void UIManager::ShowPlayerUI() {

	SDL_Texture* currentHabilityTex = nullptr;

	int playerFullHearts = Engine::GetInstance().scene->player->playerCurrentHp / 2;
	int damagedHearts = 5 - playerFullHearts;
	int halfHeart = Engine::GetInstance().scene->player->playerCurrentHp % 2;

	Engine::GetInstance().render->DrawTexture(heartContainerTexture, 200, 64, NULL, 0.0f);

	for (int i = 0; i < playerFullHearts; ++i) {
		Engine::GetInstance().render->DrawTexture(heartFullTexture, 230 + (i * 70), 74, NULL, 0.0f);
	}

	if (halfHeart == 1) {
		Engine::GetInstance().render->DrawTexture(heartHalfTexture, 230 + (playerFullHearts * 70), 74, NULL, 0.0f);
	}
	else if (damagedHearts > 0) {
		Engine::GetInstance().render->DrawTexture(heartEmptyTexture, 230 + (playerFullHearts * 70), 74, NULL, 0.0f);
	}

	for (int i = playerFullHearts + 1; i < 5; ++i) {
		Engine::GetInstance().render->DrawTexture(heartEmptyTexture, 230 + (i * 70), 74, NULL, 0.0f);
	}

	if (Engine::GetInstance().scene->player->hasPowerJump == true) {
		currentHabilityTex = habilityPowerJumpTexture;
	}
	else {
		currentHabilityTex = habilityContainerTexture;
	}
	Engine::GetInstance().render->DrawTexture(currentHabilityTex, 0, 0, NULL, 0.0f);
}


// *********************************************
// GameOver UI
// *********************************************
void UIManager::ShowDeathScreen()
{
	if (Engine::GetInstance().scene->isGameOver)
		return;

	Engine::GetInstance().scene->isGameOver = true;
	Engine::GetInstance().paused = true;

	int w, h;
	Engine::GetInstance().window->GetWindowSize(w, h);

	int buttonWidth = 290;
	int buttonHeight = 86;

	SDL_Rect tryAgainButtonRect = { ((w / 2) - buttonWidth) - 20 ,(h / 2) + 40 , buttonWidth, buttonHeight };
	SDL_Rect goToMenuButtonRect = { ((w / 2) + 10) + 20 , (h / 2) + 40 , buttonWidth, buttonHeight };

	auto tryAgainButton = CreateUIElement(UIElementType::BUTTON, 10, " TRY AGAIN ", tryAgainButtonRect, Engine::GetInstance().scene->GetScene());
	tryAgainButton->SetTexture(gameOverTryAgainButtonTexture);
	tryAgainButton->isSelected = true;

	auto goToMenuButton = CreateUIElement(UIElementType::BUTTON, 11, " GO TO MENU ", goToMenuButtonRect, Engine::GetInstance().scene->GetScene());
	goToMenuButton->SetTexture(gameOverGoToMenuButtonTexture);
}