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
#include "Player.h"
#include "Map.h"

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
	roomsMetadata["MapTemplate.tmx"] = { 0, {0,0,1557,977}, 3840, 2112 };

	roomsMetadata["MapPulmo_Sala1.tmx"] = { 1, {689,831,185,101}, 3840, 2176 };
	roomsMetadata["MapPulmo_Sala2.tmx"] = { 1, {689,728,185,103}, 3840, 2176 };
	roomsMetadata["MapPulmo_Sala3.tmx"] = { 1, {603,627,179,101}, 3840, 2176 };
	roomsMetadata["MapPulmo_Sala4.tmx"] = { 1, {515,627,88,101}, 1920, 2176 };
	roomsMetadata["MapPulmo_Sala5.tmx"] = { 1, {515,728,174,204}, 3840, 4352 };
	roomsMetadata["MapPulmo_Sala6.tmx"] = { 1, {782,500,92,228}, 1920, 4864 };
	roomsMetadata["MapPulmo_Sala7.tmx"] = { 1, {692,528,90,99}, 1920, 2176 };
	roomsMetadata["MapPulmo_Sala8.tmx"] = { 1, {604,528,88,99}, 1920, 2176 };
	roomsMetadata["MapPulmo_Sala9.tmx"] = { 1, {515,528,89,99}, 1920, 2176 };
	roomsMetadata["MapPulmo_Sala10.tmx"] = { 1, {515,325,269,203}, 5760, 4352 };
	roomsMetadata["MapPulmo_Sala11.tmx"] = { 1, {784,372,225,54}, 4800, 1088 };
	roomsMetadata["MapPulmo_Sala12.tmx"] = { 1, {602,274,182,51}, 3840, 1088 };
	roomsMetadata["MapPulmo_Sala13.tmx"] = { 1, {754,224,91,53}, 1920, 1088 };
	roomsMetadata["MapPulmo_Sala14.tmx"] = { 1, {680,126,180,98}, 3904, 2176 };
	roomsMetadata["MapPulmo_Sala16.tmx"] = { 1, {958,426,51,506}, 1920, 10880 };

	roomsMetadata["MapCor_Sala1.tmx"] = { 2, {465,144,170,87}, 1920, 1280 };
	roomsMetadata["MapCor_Sala2.tmx"] = { 2, {567,144,195,159}, 1920, 2176 };
	roomsMetadata["MapCor_Sala3.tmx"] = { 2, {567,303,195,255}, 1920, 3264 };
	roomsMetadata["MapCor_Sala4.tmx"] = { 2, {421,558,294,334}, 3840, 4325 };
	roomsMetadata["MapCor_Sala5.tmx"] = { 2, {255,743,166,149}, 1920, 2176 };
	roomsMetadata["MapCor_Sala6.tmx"] = { 2, {715,558,291,334}, 3840, 4352 };
	roomsMetadata["MapCor_Sala7.tmx"] = { 2, {1006,717,172,175}, 2176, 1920 };
	roomsMetadata["MapCor_Sala8.tmx"] = { 2, {1006,509,298,208}, 3840, 2304 };
	roomsMetadata["MapCor_Sala9.tmx"] = { 2, {839,410,100,100}, 1984, 1280 };

	roomsMetadata["MapEstomac_Sala1.tmx"] = { 3, {579,219,91,111}, 1920, 2176 };
	roomsMetadata["MapEstomac_Sala2.tmx"] = { 3, {853,219,407,111}, 8640, 2176 };
	roomsMetadata["MapEstomac_Sala3.tmx"] = { 3, {1253,219,97,405}, 1920, 8704 };
	roomsMetadata["MapEstomac_Sala4.tmx"] = { 3, {488,620,869,212}, 18560, 4352 };
	roomsMetadata["MapEstomac_Sala5.tmx"] = { 3, {1357,673,130,159}, 2880, 3264 };
	roomsMetadata["MapEstomac_Sala6.tmx"] = { 3, {484,219,97,401}, 1920, 8704 };
	roomsMetadata["MapEstomac_Sala7.tmx"] = { 3, {353,620,135,212}, 2880, 1536 };
	roomsMetadata["MapEstomac_Sala8.tmx"] = { 3, {114,725,239,107}, 5120, 2176 };
	roomsMetadata["MapEstomac_Sala9.tmx"] = { 3, {581,330,358,290}, 7680, 4352 };
	roomsMetadata["MapEstomac_Sala10.tmx"] = { 3, {70,675,94,55}, 1920, 1088 };

	roomsMetadata["MapMedula_Sala1.tmx"] = { 4, {698,816,163,119}, 5760, 4352 };
	roomsMetadata["MapMedula_Sala2.tmx"] = { 4, {698,580,163,118}, 5760, 4352 };
	roomsMetadata["MapMedula_Sala3.tmx"] = { 4, {698,351,163,118}, 5760, 4352 };
	roomsMetadata["MapMedula_Sala4.tmx"] = { 4, {111,111,111,111}, 5760, 4352 };
	roomsMetadata["MapMedula_Sala5.tmx"] = { 4, {111,111,111,111}, 5760, 4352 };
	roomsMetadata["MapMedula_SalaBoss.tmx"] = { 5, {632,123,293,123}, 3200, 2944 };
	
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
	life_anims.Update(dt);

	if (Engine::GetInstance().scene->GetCurrentScene() == SceneID::MAIN_MENU && currentMenuState == MainMenuState::MAIN_BUTTONS)
	{
		titleAnim.Update(dt);
	}

	if (currentPauseState == PauseMenuState::INVENTORY || currentPauseState == PauseMenuState::INVENTORY2)
	{
		itemAmigdalaAnim.Update(dt);
		itemSalivaAnim.Update(dt);
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
	sliderMusicTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/MusicIcon.png");
	sliderSFXTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SFXIcon.png");

	titleAnimTexture = Engine::GetInstance().textures->Load("Assets/Textures/Backgrounds/MainMenu/AnimatedTitle.png");
	for (int i = 0; i < 32; ++i)
	{
		int col = i % 16;
		int row = i / 16;

		SDL_Rect frameRect = { col * 854, row * 142, 854, 142 };

		titleAnim.AddFrame(frameRect, 80);
	}
	titleAnim.SetLoop(true);
	
	
	/* Pause UI*/
	pauseOptionsMenuTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Fondo_pause_menu.png");
	inventoryPg1Texture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameMenus/Inventory/InventoryPg1.png");
	inventoryPg2Texture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameMenus/Inventory/InventoryPg2.png");
	inventoryNextBtnTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/Inventario_flecha derecha.png");
	inventoryPrevBtnTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/Inventario_flecha izquierda.png");

	mapWindowTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameMenus/Map/BaseScreen.png");
	playerIconTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderKnob.png");
	mapTextureMouth = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameMenus/Map/MouthMap.png");
	mapTextureLung = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameMenus/Map/LungsMap.png");
	mapTextureHeart = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameMenus/Map/HeartMap.png");
	mapTextureStomach = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameMenus/Map/StomachMap.png");
	mapTextureMarrow = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameMenus/Map/MarrowMap.png"); 
	mapTextureMarrow2 = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameMenus/Map/MarrowMap2.png");
	mapTextureBrain = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameMenus/Map/BrainMap.png");

	powerupsTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PowerUps_Menu.png");
	continuePauseButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/ContinueButton.png");
	optionsPauseButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/OptionsButton.png");
	menuQuitPauseButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/QuitToMenuButton.png");
	gameQuitButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/QuitGameButton.png");

	inventoryTabButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/BlankButtonTex.png");
	itemAmigdalaTex = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Amigdala_Coleccionable.png");
	itemSalivaTex = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Saliva_Coleccionable.png");

	itemTejidoPulmonarTex = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/TejidoAlveolar_Coleccionable.png");
	itemCiliosTex = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Cilios_Coleccionables.png");
	itemMucosaPulmonarTex = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/ExtractoMucosaPulmonar_Coleccionable.png");
	itemNeocitoTex = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Neumocito_Coleccionable.png");
	itemTejidoNecroTex = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Tejido_Necrotizado_Coleccionable.png");
	itemRamaTex = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Rama_Coleccionable.png");

	itemCelulasParietalesTex = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/ExtractoCelulaParietal_Coleccionable.png");
	itemCelulasEpitelailesTex = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Extracto_celula_Coleccionable_epitelial.png");
	itemTejidoEstomacalTex = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/TejidoEstomacal_Coleccionable.png");
	itemMucosaGastricaTex = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/MucosaGastrica_Coleccioable.png");
	itemUlcerasTex = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Formacion_Ulcerosa_Coleccionable.png");
	itemMasaTex = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/Masa_Carcomida_Coleccionable.png");
	itemCrisalidaTex = Engine::GetInstance().textures->Load("Assets/Textures/Collectibles/ExtractoCrisalida_Coleccionable.png");

	for (int i = 0; i < 9; ++i) {
		itemTejidoEstomacalAnim.AddFrame({ i * 32, 0, 32, 32 }, 100);
		itemUlcerasAnim.AddFrame({ i * 32, 0, 32, 32 }, 100);
	}
	itemTejidoEstomacalAnim.SetLoop(true);
	itemUlcerasAnim.SetLoop(true);

	for (int i = 0; i < 10; ++i) {
		itemCelulasParietalesAnim.AddFrame({ i * 32, 0, 32, 32 }, 100);
	}
	itemCelulasParietalesAnim.SetLoop(true);

	for (int i = 0; i < 11; ++i) {
		itemAmigdalaAnim.AddFrame({ i * 32, 0, 32, 32 }, 100);
		itemSalivaAnim.AddFrame({ i * 32, 0, 32, 32 }, 100);
		itemTejidoPulmonarAnim.AddFrame({ i * 32, 0, 32, 32 }, 100);
	}
	itemAmigdalaAnim.SetLoop(true);
	itemSalivaAnim.SetLoop(true);
	itemTejidoPulmonarAnim.SetLoop(true);

	for (int i = 0; i < 12; ++i) {
		itemMucosaGastricaAnim.AddFrame({ i * 32, 0, 32, 32 }, 100);
	}
	itemMucosaGastricaAnim.SetLoop(true);

	for (int i = 0; i < 13; ++i) {
		itemMasaAnim.AddFrame({ i * 32, 0, 32, 32 }, 100);
	}
	itemMasaAnim.SetLoop(true);

	for (int i = 0; i < 14; ++i) {
		itemCiliosAnim.AddFrame({ i * 32, 0, 32, 32 }, 100);
		itemRamaAnim.AddFrame({ i * 32, 0, 32, 32 }, 100);
	}
	itemCiliosAnim.SetLoop(true);
	itemRamaAnim.SetLoop(true);

	for (int i = 0; i < 18; ++i) {
		itemTejidoNecroAnim.AddFrame({ i * 32, 0, 32, 32 }, 100);
		itemCrisalidaAnim.AddFrame({ i * 32, 0, 32, 32 }, 100);
	}
	itemTejidoNecroAnim.SetLoop(true);
	itemCrisalidaAnim.SetLoop(true);

	for (int i = 0; i < 19; ++i) {
		itemMucosaPulmonarAnim.AddFrame({ i * 32, 0, 32, 32 }, 100);
	}
	itemMucosaPulmonarAnim.SetLoop(true);

	for (int i = 0; i < 21; ++i) {
		itemNeocitoAnim.AddFrame({ i * 32, 0, 32, 32 }, 100);
	}
	itemNeocitoAnim.SetLoop(true);

	for (int i = 0; i < 30; ++i) {
		itemCelulasEpitelailesAnim.AddFrame({ i * 32, 0, 32, 32 }, 100);
	}
	itemCelulasEpitelailesAnim.SetLoop(true);

	minimapTabButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/BlankButtonTex.png");
	powerUpsTabButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/PauseMenu_Buttons/BlankButtonTex.png");

	/*Player UI*/
	/*heartFullTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Corazon_full.png");
	heartHalfTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Corazon_meitat.png");
	heartEmptyTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Corazon_muerto.png");
	heartContainerTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Caja_vida.png");*/
	std::unordered_map<int, std::string> aliases = { {0, "10"}, {1, "9"}, {2, "8"}, {3, "7"}, {4, "6"}, {5, "5"}, {6, "4"}, {7, "3"}, {8, "2"}, {9, "1"}, {10, "0"} };
	lifeTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/InGameUI/Atlas_vida.png");
	life_anims.LoadFromTSX("Assets/Textures/UI/InGameUI/Atlas_vida.tsx", aliases);
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

	int bigButtonWidth = 385;
	int bigButtonHeight = 115;
	int buttonWidth = 290;
	int buttonHeight = 86;

	// Instantiate a UIButton in the Scene
	SDL_Rect playButtonRect = { 146 , 421 , bigButtonWidth, bigButtonHeight };
	SDL_Rect optionsButtonRect = { 149 , 628 , buttonWidth, buttonHeight };
	SDL_Rect exitButtonRect = { 148 , 809 , buttonWidth, buttonHeight };

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
		Engine::GetInstance().render->DrawTexture(sliderMusicTexture, ((w - sliderMusicTexture->w) / 2) - 200, ((h - sliderMusicTexture->h) / 2) - 65, NULL, 0.0f);

		Engine::GetInstance().render->DrawTexture(sliderBoxTexture, (w - sliderBoxTexture->w) / 2, ((h - (sliderBoxTexture->h)) / 2) + 100, NULL, 0.0f);
		Engine::GetInstance().render->DrawTexture(sliderSFXTexture, ((w - sliderSFXTexture->w) / 2) - 200, ((h - sliderSFXTexture->h) / 2) + 100, NULL, 0.0f);

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

		if (titleAnimTexture != nullptr)
		{
			SDL_Rect currentFrame = titleAnim.GetCurrentFrame();

			int w, h;
			Engine::GetInstance().window->GetWindowSize(w, h);

			int drawX = 117;
			int drawY = 135;

			Engine::GetInstance().render->DrawTexture(titleAnimTexture, drawX, drawY, &currentFrame, 0.0f);
		}
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
	musicSlider->SetValue((int)(Engine::GetInstance().audio->GetMusicVolume() * 100.0f));

	auto fxSlider = std::static_pointer_cast<UISlider>(fxSliderElement);
	fxSlider->SetTexture(sliderBarTexture);
	fxSlider->SetKnobTexture(sliderKnobTexture);
	fxSlider->SetValue((int)(Engine::GetInstance().audio->GetSFXVolume() * 100.0f));

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

		Engine::GetInstance().scene->isContinuing = false;
		//Engine::GetInstance().scene->StartFadeToScene(SceneID::LEVEL, 0.5f);

		Engine::GetInstance().scene->PlayVideo("AnimaticaIntro");
		break;
	}
	case 9: // CONTINUE BUTTON
	{
		LOG("CONTINUING GAME");		

		Engine::GetInstance().scene->isContinuing = true;
		Engine::GetInstance().scene->StartFadeToScene(SceneID::LEVEL, 0.5f);

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

	if (currentPauseState == PauseMenuState::INVENTORY || currentPauseState == PauseMenuState::INVENTORY2 || currentPauseState == PauseMenuState::MINIMAP || currentPauseState == PauseMenuState::POWERUPS)
	{
		SDL_Texture* texToDraw = nullptr;

		
		if (currentPauseState == PauseMenuState::INVENTORY) texToDraw = inventoryPg1Texture;
		else if (currentPauseState == PauseMenuState::INVENTORY2) texToDraw = inventoryPg2Texture;
		else if (currentPauseState == PauseMenuState::MINIMAP)
		{
			if (mapWindowTexture != nullptr)
			{
				int bgW, bgH;
				Engine::GetInstance().textures->GetSize(mapWindowTexture, bgW, bgH);
				Engine::GetInstance().render->DrawTexture(mapWindowTexture, (w / 2) - (bgW / 2), (h / 2) - (bgH / 2), NULL, 0.0f);
			}

			std::string currentMap = Engine::GetInstance().map->mapFileName;
			RoomMetadata currentRoomData = roomsMetadata[currentMap];

			if (player != nullptr && !currentMap.empty())
			{
				player->visitedRooms.insert(currentMap);
			}

			SDL_Texture* zoneMapTexture = nullptr;
			switch (currentRoomData.zoneID)
			{
			case 0: zoneMapTexture = mapTextureMouth; break;
			case 1: zoneMapTexture = mapTextureLung; break;
			case 2: zoneMapTexture = mapTextureHeart; break;
			case 3: zoneMapTexture = mapTextureStomach; break;
			case 4: zoneMapTexture = mapTextureMarrow; break;
			case 5: zoneMapTexture = mapTextureMarrow2; break;
			case 6: zoneMapTexture = mapTextureBrain; break;
			}

			int zoneW, zoneH;
			Engine::GetInstance().textures->GetSize(zoneMapTexture, zoneW, zoneH);
			int mapDrawX = (w / 2) - (zoneW / 2);
			int mapDrawY = (h / 2) - (zoneH / 2);

			SDL_Rect alwaysVisibleRect = { 55, 133, 260, 270 };
			int destAlwaysX = mapDrawX + alwaysVisibleRect.x;
			int destAlwaysY = mapDrawY + alwaysVisibleRect.y;
			Engine::GetInstance().render->DrawTexture(zoneMapTexture, destAlwaysX, destAlwaysY, &alwaysVisibleRect, 0.0f);

			for (const std::string& roomName : player->visitedRooms)
			{
				if (roomsMetadata.find(roomName) != roomsMetadata.end() && roomsMetadata[roomName].zoneID == currentRoomData.zoneID)
				{
					RoomMetadata roomData = roomsMetadata[roomName];

					int destX = mapDrawX + roomData.mapRect.x;
					int destY = mapDrawY + roomData.mapRect.y;

					Engine::GetInstance().render->DrawTexture(zoneMapTexture, destX, destY, &roomData.mapRect, 0.0f);
				}
			}

			if (player != nullptr && playerIconTexture != nullptr)
			{
				Vector2D playerPos = player->GetPosition();

				float percentX = playerPos.getX() / (float)currentRoomData.worldWidth;
				float percentY = playerPos.getY() / (float)currentRoomData.worldHeight;

				int iconLocalX = (int)(percentX * currentRoomData.mapRect.w);
				int iconLocalY = (int)(percentY * currentRoomData.mapRect.h);

				int finalIconX = mapDrawX + currentRoomData.mapRect.x + iconLocalX;
				int finalIconY = mapDrawY + currentRoomData.mapRect.y + iconLocalY;

				int iconW = 49;
				int iconH = 49;

				Engine::GetInstance().render->DrawTexture(playerIconTexture, finalIconX - (iconW / 2), finalIconY - (iconH / 2), NULL, 0.0f);
			}
		}
		else if (currentPauseState == PauseMenuState::POWERUPS) texToDraw = powerupsTexture;

		if (texToDraw != nullptr) {
			int texW, texH;
			Engine::GetInstance().textures->GetSize(texToDraw, texW, texH);
			Engine::GetInstance().render->DrawTexture(texToDraw, (w / 2) - (texW / 2), (h / 2) - (texH / 2), NULL, 0.0f);
		}

		if (currentPauseState == PauseMenuState::INVENTORY || currentPauseState == PauseMenuState::INVENTORY2)
		{

			auto player = Engine::GetInstance().scene->player;
			if (player != nullptr)
			{
				int startX = 172;
				int startY = 148;
				int offsetX = 93;
				int offsetY = 100;

				for (size_t i = 0; i < player->list_collectibles.size(); i++)
				{
					int c_num = player->list_collectibles[i];

					bool drawThisPage = false;
					int drawX = 0;
					int drawY = 0;

					if (c_num >= 0 && c_num <= 1) // MOUTH (Row 1, Pag 1)
					{
						if (currentPauseState == PauseMenuState::INVENTORY) {
							drawThisPage = true;
							drawX = startX + (c_num * offsetX);
							drawY = startY;
						}
					}
					else if (c_num >= 2 && c_num <= 7) // LUNGS (Row 2, Pag 1)
					{
						if (currentPauseState == PauseMenuState::INVENTORY) {
							drawThisPage = true;
							drawX = startX + ((c_num - 2) * offsetX);
							drawY = startY + offsetY;
						}
					}
					else if (c_num >= 8 && c_num <= 12) // HEART (Row 3, Pag 1)
					{
						if (currentPauseState == PauseMenuState::INVENTORY) {
							drawThisPage = true;
							drawX = startX + ((c_num - 8) * offsetX);
							drawY = startY + (offsetY * 2);
						}
					}
					else if (c_num >= 13 && c_num <= 19) // STOMACH (Row 1, Pag 2)
					{
						if (currentPauseState == PauseMenuState::INVENTORY2) {
							drawThisPage = true;
							drawX = startX + ((c_num - 13) * offsetX);
							drawY = startY;
						}
					}
					else if (c_num >= 20 && c_num <= 24) // BRAIN (Row 2, Pag 2)
					{
						if (currentPauseState == PauseMenuState::INVENTORY2) {
							drawThisPage = true;
							drawX = startX + ((c_num - 20) * offsetX);
							drawY = startY + offsetY;
						}
					}

					if (drawThisPage)
					{
						SDL_Texture* animTexToDraw = nullptr;
						SDL_Rect currentFrame = { 0,0,0,0 };

						if (c_num == 0)
						{
							animTexToDraw = itemAmigdalaTex;
							currentFrame = itemAmigdalaAnim.GetCurrentFrame();
						}
						else if (c_num == 1)
						{
							animTexToDraw = itemSalivaTex;
							currentFrame = itemSalivaAnim.GetCurrentFrame();
						}
						else if (c_num == 2)
						{
							animTexToDraw = itemTejidoPulmonarTex;
							currentFrame = itemTejidoPulmonarAnim.GetCurrentFrame();
						}
						else if (c_num == 3)
						{
							animTexToDraw = itemCiliosTex;
							currentFrame = itemCiliosAnim.GetCurrentFrame();
						}
						else if (c_num == 4)
						{
							animTexToDraw = itemMucosaPulmonarTex;
							currentFrame = itemMucosaPulmonarAnim.GetCurrentFrame();
						}
						else if (c_num == 5)
						{
							animTexToDraw = itemNeocitoTex;
							currentFrame = itemNeocitoAnim.GetCurrentFrame();
						}
						else if (c_num == 6)
						{
							animTexToDraw = itemTejidoNecroTex;
							currentFrame = itemTejidoNecroAnim.GetCurrentFrame();
						}
						else if (c_num == 7)
						{
							animTexToDraw = itemRamaTex;
							currentFrame = itemRamaAnim.GetCurrentFrame();
						}
						else if (c_num == 13)
						{
							animTexToDraw = itemCelulasParietalesTex;
							currentFrame = itemCelulasParietalesAnim.GetCurrentFrame();
						}
						else if (c_num == 14)
						{
							animTexToDraw = itemCelulasEpitelailesTex;
							currentFrame = itemCelulasEpitelailesAnim.GetCurrentFrame();
						}
						else if (c_num == 15)
						{
							animTexToDraw = itemTejidoEstomacalTex;
							currentFrame = itemTejidoEstomacalAnim.GetCurrentFrame();
						}
						else if (c_num == 16)
						{
							animTexToDraw = itemMucosaGastricaTex;
							currentFrame = itemMucosaGastricaAnim.GetCurrentFrame();
						}
						else if (c_num == 17)
						{
							animTexToDraw = itemUlcerasTex;
							currentFrame = itemUlcerasAnim.GetCurrentFrame();
						}
						else if (c_num == 18)
						{
							animTexToDraw = itemMasaTex;
							currentFrame = itemMasaAnim.GetCurrentFrame();
						}
						else if (c_num == 19)
						{
							animTexToDraw = itemCrisalidaTex;
							currentFrame = itemCrisalidaAnim.GetCurrentFrame();
						}
						// Add here the rest of collectibles with their corresponding animations when we have their .png and .tsx!!!

						if (animTexToDraw != nullptr)
						{
							Engine::GetInstance().render->DrawTexture(animTexToDraw, drawX, drawY, &currentFrame, 0.0f, 0.0, 0, 0, SDL_FLIP_NONE, 2.0f);
						}
					}
				}
			}
		}

		return;
	}

	int imgWidth = 855;
	int imgHeight = 839;

	int pauseOptionsMenuPosX = (w - imgWidth) / 2;
	int pauseOptionsMenuPosY = (h - imgHeight) / 2;

	Engine::GetInstance().render->DrawTexture(pauseOptionsMenuTexture, pauseOptionsMenuPosX, pauseOptionsMenuPosY, NULL, 0.0f);

	if (currentPauseState == PauseMenuState::OPTIONS)
	{
		Engine::GetInstance().render->DrawTexture(sliderBoxTexture, (w - sliderBoxTexture->w) / 2, (h - (sliderBoxTexture->h * 2)) / 2, NULL, 0.0f);
		Engine::GetInstance().render->DrawTexture(sliderMusicTexture, ((w - sliderMusicTexture->w) / 2) - 200, ((h - sliderMusicTexture->h) / 2) - 65, NULL, 0.0f);

		Engine::GetInstance().render->DrawTexture(sliderBoxTexture, (w - sliderBoxTexture->w) / 2, ((h - (sliderBoxTexture->h)) / 2) + 80, NULL, 0.0f);
		Engine::GetInstance().render->DrawTexture(sliderSFXTexture, ((w - sliderSFXTexture->w) / 2) - 200, ((h - sliderSFXTexture->h) / 2) + 80, NULL, 0.0f);

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

void UIManager::LoadInventoryTab()
{
	CleanUp();
	currentPauseState = PauseMenuState::INVENTORY;
	lastActiveTab = PauseMenuState::INVENTORY;

	SDL_Rect mapButtonBounds = { 188, 58, 280, 66 }; 
	SDL_Rect powerUpsButtonBounds = { 780, 58, 280, 66 };
	SDL_Rect nextButtonBounds = { 1632, 933, 66, 46 };

	auto mapButton = CreateUIElement(UIElementType::BUTTON, 20, " ", mapButtonBounds, Engine::GetInstance().scene->GetScene());
	mapButton->SetTexture(minimapTabButtonTexture);

	auto powerUpsButton = CreateUIElement(UIElementType::BUTTON, 21, " ", powerUpsButtonBounds, Engine::GetInstance().scene->GetScene());
	powerUpsButton->SetTexture(powerUpsTabButtonTexture);

	auto nextButton = CreateUIElement(UIElementType::BUTTON, 23, " ", nextButtonBounds, Engine::GetInstance().scene->GetScene());
	nextButton->SetTexture(inventoryNextBtnTexture);
}

void UIManager::LoadInventoryTab2()
{
	CleanUp();
	currentPauseState = PauseMenuState::INVENTORY2;
	lastActiveTab = PauseMenuState::INVENTORY2;

	SDL_Rect mapButtonBounds = { 188, 58, 280, 66 };
	SDL_Rect powerUpsButtonBounds = { 780, 58, 280, 66 };
	SDL_Rect prevButtonBounds = { 220, 933, 66, 46 };

	auto mapButton = CreateUIElement(UIElementType::BUTTON, 20, " ", mapButtonBounds, Engine::GetInstance().scene->GetScene());
	mapButton->SetTexture(minimapTabButtonTexture);

	auto powerUpsButton = CreateUIElement(UIElementType::BUTTON, 21, " ", powerUpsButtonBounds, Engine::GetInstance().scene->GetScene());
	powerUpsButton->SetTexture(powerUpsTabButtonTexture);

	auto prevButton = CreateUIElement(UIElementType::BUTTON, 24, " ", prevButtonBounds, Engine::GetInstance().scene->GetScene());
	prevButton->SetTexture(inventoryPrevBtnTexture);
}


void UIManager::LoadMinimapTab()
{
	CleanUp();
	currentPauseState = PauseMenuState::MINIMAP;
	lastActiveTab = PauseMenuState::MINIMAP;

	SDL_Rect inventoryButtonBounds = { 480, 58, 280, 66 };
	SDL_Rect powerUpsButtonBounds = { 780, 58, 280, 66 };

	auto invBtn = CreateUIElement(UIElementType::BUTTON, 22, " ", inventoryButtonBounds, Engine::GetInstance().scene->GetScene());
	invBtn->SetTexture(inventoryTabButtonTexture);

	auto pwrBtn = CreateUIElement(UIElementType::BUTTON, 21, " ", powerUpsButtonBounds, Engine::GetInstance().scene->GetScene());
	pwrBtn->SetTexture(powerUpsTabButtonTexture);
}

void UIManager::LoadPowerUpsTab()
{
	CleanUp();
	currentPauseState = PauseMenuState::POWERUPS;
	lastActiveTab = PauseMenuState::POWERUPS;

	SDL_Rect mapButtonBounds = { 188, 58, 280, 66 };
	SDL_Rect inventoryButtonBounds = { 480, 58, 280, 66 };

	auto mapButton = CreateUIElement(UIElementType::BUTTON, 20, " ", mapButtonBounds, Engine::GetInstance().scene->GetScene());
	mapButton->SetTexture(minimapTabButtonTexture);

	auto invBtn = CreateUIElement(UIElementType::BUTTON, 22, " ", inventoryButtonBounds, Engine::GetInstance().scene->GetScene());
	invBtn->SetTexture(inventoryTabButtonTexture);
}

void UIManager::LoadLastActiveTab()
{
	switch (lastActiveTab)
	{
	case PauseMenuState::INVENTORY:
		LoadInventoryTab();
		break;
	case PauseMenuState::INVENTORY2:
		LoadInventoryTab2();
		break;
	case PauseMenuState::MINIMAP:
		LoadMinimapTab();
		break;
	case PauseMenuState::POWERUPS:
		LoadPowerUpsTab();
		break;
	default:
		LoadInventoryTab();
		break;
	}
}

void UIManager::HandleUINavigation(int initialID, int finalID, MenuNavDirection direction) {
	/* MENU NAVIGATION WITH KEYS */

	auto selectedUI = GetElement(selectedUIID);
	if (!selectedUI) return;
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
	case 20: // Go to MINIMAP
		LoadMinimapTab();
		break;
	case 21: // Go to POWERUPS
		LoadPowerUpsTab();
		break;
	case 22: // Go to INVENTORY
		LoadInventoryTab();
		break;
	case 23: // Go to INVENTORY PAGE 2
		LoadInventoryTab2();
		break;
	case 24: // Go to INVENTORY PAGE 1 (from page 2)
		LoadInventoryTab();
		break;
	}
}


// *********************************************
// Player UI
// *********************************************
void UIManager::ShowPlayerUI() {
	if (player != nullptr && player == Engine::GetInstance().scene->player){
		changeLifeAnim(player->playerCurrentHp);
		const SDL_Rect& animFrame = life_anims.GetCurrentFrame();
		float texW = animFrame.w;
		float texH = animFrame.h;
		Engine::GetInstance().render->DrawTexture(lifeTexture, 200, 64, &animFrame, 0.0f);
	}
	else {
		player = Engine::GetInstance().scene->player;
	}
	
	SDL_Texture* currentHabilityTex = nullptr;
	
	/*int playerFullHearts = Engine::GetInstance().scene->player->playerCurrentHp / 2;
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
	}*/

	if (Engine::GetInstance().scene->player->hasPowerJump == true) {
		currentHabilityTex = habilityPowerJumpTexture;
	}
	else {
		currentHabilityTex = habilityContainerTexture;
	}
	Engine::GetInstance().render->DrawTexture(currentHabilityTex, 0, 0, NULL, 0.0f);
	
	//Chuleta
	if (player->IsGodMode()) {
		int letter_size = 14;
		std::string line1 = "--------- Controls ---------";
		std::string line2 = "Movement: A / D | Left Analog";
		std::string line3 = "Jump : Space | B";
		std::string line4 = "Stun attack : Left Click | X";
		std::string line5 = "Suck attack : Right Click | Y";
		std::string line6 = "Dash : E";
		std::string line7 = "Crouch : Left Shift";

		Engine::GetInstance().render->DrawText(line1.c_str(), 200, 200, line1.size() * letter_size, 30, SDL_Color{ 255,255,0,255});
		Engine::GetInstance().render->DrawText(line2.c_str(), 200, 225, line2.size() * letter_size, 30, SDL_Color{ 255,255,0,255 });
		Engine::GetInstance().render->DrawText(line3.c_str(), 200, 250, line3.size() * letter_size, 30, SDL_Color{ 255,255,0,255 });
		Engine::GetInstance().render->DrawText(line4.c_str(), 200, 275, line4.size() * letter_size, 30, SDL_Color{ 255,255,0,255 });
		Engine::GetInstance().render->DrawText(line5.c_str(), 200, 300, line5.size() * letter_size, 30, SDL_Color{ 255,255,0,255 });
		Engine::GetInstance().render->DrawText(line6.c_str(), 200, 325, line6.size() * letter_size, 30, SDL_Color{ 255,255,0,255 });
		Engine::GetInstance().render->DrawText(line7.c_str(), 200, 350, line7.size() * letter_size, 30, SDL_Color{ 255,255,0,255 });			
	}
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

void UIManager::changeLifeAnim(int life)
{
	std::string anim;
	switch (life)
	{
		case 10:
			anim = "10";
			break;
		case 9:
			anim = "9";
			break;
		case 8:
			anim = "8";
			break;
		case 7:
			anim = "7";
			break;
		case 6:
			anim = "6";
			break;
		case 5:
			anim = "5";
			break;
		case 4:
			anim = "4";
			break;
		case 3:
			anim = "3";
			break;
		case 2:
			anim = "2";
			break;
		case 1:
			anim = "1";
			break;
		case 0:
			anim = "0";
			break;
		default:
			break;
	}
	life_anims.SetCurrent(anim);
}