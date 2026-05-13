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
	Engine::GetInstance().uiManager->LoadUITextures();

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
			ChangeScene(SceneID::LEVEL);
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
	case SceneID::LEVEL:
		UpdateLevel(dt);
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
	case SceneID::LEVEL:
		PostUpdateLevel();
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
		Engine::GetInstance().uiManager->HandleMainMenuUIEvents(uiElement);
		break;
	case SceneID::LEVEL:
		Engine::GetInstance().uiManager->HandlePauseMenuUIEvents(uiElement);
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

	case SceneID::LEVEL:
		LoadLevel("MapTemplate");
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

	case SceneID::LEVEL:
		UnloadLevel();
		break;

	}
}

// *********************************************
// MAIN MENU functions
// *********************************************

void Scene::LoadMainMenu() {

	//Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/retro-gaming-short-248416.wav");

	/* Load all needed textures */

	/*mainMenuBackground = Engine::GetInstance().textures->Load("Assets/Textures/UI/menu INCORPUS.png");
	sliderBarTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderBar.png");
	sliderKnobTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderKnob.png");
	backButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/BackButton.png");
	playButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/PlayButton.png");
	optionsButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/OptionsButton.png");
	exitButtonTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/MainMenu_Buttons/ExitButton.png");
	sliderBoxTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/SliderBox.png");
	sliderAudioTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Sliders/AudioIcon.png");*/
	
	Engine::GetInstance().uiManager->LoadMainMenuButtons();
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
	
	Engine::GetInstance().uiManager->ShowMainMenuButtons();
	//int firstElement = 0;
	//int lastElement = 0;

	//if (mainMenuBackground != nullptr)
	//{
	//	Engine::GetInstance().render->DrawTexture(mainMenuBackground, 0, 0, NULL, 0.0f);
	//}

	//if (currentMenuState == MainMenuState::OPTIONS) {
	//	int w, h;
	//	
	//	Engine::GetInstance().window->GetWindowSize(w, h);

	//	SDL_Rect fullscreenRect = { 0, 0, w, h };

	//	Engine::GetInstance().render->DrawRectangle(fullscreenRect, 0, 0, 0, 150, true, false);

	//	Engine::GetInstance().render->DrawTexture(sliderBoxTexture, (w - sliderBoxTexture->w) / 2, (h - (sliderBoxTexture->h * 2)) / 2, NULL, 0.0f);
	//	Engine::GetInstance().render->DrawTexture(sliderAudioTexture, ((w - sliderAudioTexture->w) / 2) - 200, ((h - sliderAudioTexture->h) / 2) - 65, NULL, 0.0f);

	//	Engine::GetInstance().render->DrawTexture(sliderBoxTexture, (w - sliderBoxTexture->w) / 2, ((h - (sliderBoxTexture->h)) / 2) + 100, NULL, 0.0f);
	//	Engine::GetInstance().render->DrawTexture(sliderAudioTexture, ((w - sliderAudioTexture->w) / 2) - 200, ((h - sliderAudioTexture->h) / 2) + 100, NULL, 0.0f);

	//	firstElement = 4;
	//	lastElement = 7;
	//	
	//}
	//else {
	//	firstElement = 1;
	//	lastElement = 3;
	//}

	///* UI CONTROLS */
	//if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_DPAD_UP) == KEY_DOWN)
	//	HandleUINavigation(firstElement, lastElement, MenuNavDirection::UP);
	//else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_DPAD_DOWN) == KEY_DOWN)
	//	HandleUINavigation(firstElement, lastElement, MenuNavDirection::DOWN);
	//else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT || Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_DPAD_LEFT) == KEY_DOWN)
	//	HandleUINavigation(firstElement, lastElement, MenuNavDirection::LEFT);
	//else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT || Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_DPAD_RIGHT) == KEY_DOWN)
	//	HandleUINavigation(firstElement, lastElement, MenuNavDirection::RIGHT);

}

// *********************************************
// PAUSE MENU functions
// *********************************************

void Scene::HandlePause() {
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_START) == KEY_DOWN) {
		Engine::GetInstance().Func_PauseEngine();

		if (Engine::GetInstance().paused) {
			Engine::GetInstance().uiManager->LoadPauseMenu();
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

void Scene::LoadInventoryMenu()
{
	Engine::GetInstance().uiManager->CleanUp();
	currentPauseState = PauseMenuState::INVENTORY;	

	pauseOptionsMenuTexture = Engine::GetInstance().textures->Load("Assets/Textures/UI/Fondo_pause_menu.png");
}

//void Scene::UnloadPauseMenu()
//{
//
//}

// *********************************************
// Level 1 functions
// *********************************************

void Scene::LoadLevel(std::string level, float playerX, float playerY) {

	//Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/level-iv-339695.wav");

	//Call the function to load the map. 
	std::string map = level + ".tmx";
	Engine::GetInstance().map->Load("Assets/Maps/", map);

	//Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player);

	if (playerX && playerY) {
		player->SetPosition(Vector2D(playerX, playerY));
		player->position = Vector2D(playerX, playerY);
	}
}

void Scene::UpdateLevel(float dt) {

	HandlePause();
	if (Engine::GetInstance().paused) {
		Engine::GetInstance().map->DrawForeground();
		Engine::GetInstance().uiManager->ShowPauseMenu();
	}
	else {
		if (player->canDialog) {

		}
	}
}

void Scene::UnloadLevel() {

	// Clean up UI elements related to the Level1
	auto& uiManager = Engine::GetInstance().uiManager;
	uiManager->CleanUp();


	// Clean up map and entities
	Engine::GetInstance().map->CleanUp();
	Engine::GetInstance().entityManager->CleanUp(false);

	destroyedEntitiesIds.clear();

}

void Scene::PostUpdateLevel() {

	if (!Engine::GetInstance().paused)
	{
		Engine::GetInstance().map->DrawForeground();
		if (player && player->active)
		{
			Engine::GetInstance().uiManager->ShowPlayerUI();
		}
	}

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
		ChangeScene(SceneID::LEVEL);
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
	Engine::GetInstance().entityManager->CleanUp(false);

	destroyedEntitiesIds.clear();
}

void Scene::ActivateGameOver()
{
	Engine::GetInstance().uiManager->ShowDeathScreen();
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
		ChangeScene(SceneID::LEVEL); // Fallback: if failed to load video, goes directly to level 1
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
	if (playerNode) 
	{
		if (player == nullptr) {
			player = std::dynamic_pointer_cast<Player>(Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER));
			player->Start(); //L17: Important to call Start to initialize teh Entity
		}
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