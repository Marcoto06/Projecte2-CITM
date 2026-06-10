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
#include "Boss2.h"

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
	LoadVideo(&introVideo, "AnimaticaIntro");
	LoadVideo(&loadingVideo, "LoadingScreen");
	LoadVideo(&fallingVideo, "AnimCaida");
	LoadVideo(&finalVideo, "AnimaticaEnd");
	LoadVideo(&finalVideov2, "AnimaticaEndv2");

	//Audio fx
	
	loadingScreenID = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx UI/pantalla_de_carrega.wav");
	latidosFXId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/latidos.wav");
	fondoBocaFXId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/sonido de fondo 1.wav");
	fondoPulmonesFXId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Pulmones_Background.wav");
	fondoEstomagoFXId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/stomach_fondo.wav");
	fondoMedulaFXId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/medula_fondo.wav");
	clicBotoFXId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Fx UI/Clic_Boto.wav");

	LoadScene(currentScene); // start in MAIN_MENU
	//Engine::GetInstance().audio->PlayFx(latidosFXId, 50);
	Engine::GetInstance().audio->PlayFx(loadingScreenID);
	//Engine::GetInstance().audio->PlayFx(fondoBocaFXId, 50);
	
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
		LOG("Video dt: %f", dt);
		plm_decode(currentVideo.plm, dt / 1000.0f);	// pl_mpeg uses time in seconds, dt is in milliseconds

		if (currentVideo.texture && currentVideo.buffer) {

			SDL_UpdateTexture(currentVideo.texture, NULL, currentVideo.buffer, currentVideo.width * 4);
			SDL_RenderTexture(Engine::GetInstance().render->renderer, currentVideo.texture, NULL, NULL);
			
		}

		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || plm_has_ended(currentVideo.plm) || videoTimer.ReadSec() >= currentVideo.duration)
		{
			
			if (transitionAfterVideo)
			{
				transitionAfterVideo = false;
				if (player != nullptr) player->lock = false;

				StopCurrentVideo();
				StartFadeToMap(videoNextMap, videoNextX, videoNextY);
			}
			else
			{
				std::string finishedVideoName = currentVideo.file;

				StopCurrentVideo();

				if (finishedVideoName == "AnimaticaIntro")
				{
					StartFadeToScene(SceneID::LEVEL, 0.5f);
				}
				else if (finishedVideoName == "AnimaticaEnd" || finishedVideoName == "AnimaticaEndv2")
				{
					StartFadeToScene(SceneID::MAIN_MENU, 0.5f);
				}
				else if (currentScene == SceneID::INTRO_SCREEN)
				{
					ChangeScene(SceneID::LEVEL);
				}
			}

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
		if (!isPlayingVideo) 
		{
			UpdateLevel(dt);
		}
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

	if (fadeState != FadeState::NONE)
	{
		float dt = Engine::GetInstance().GetDt();

		float fadeSpeed = (255.0f / (fadeDuration * 1000.0f)) * dt;

		if (fadeState != FadeState::NONE)
		{
			float dt = Engine::GetInstance().GetDt();
			float fadeSpeed = (255.0f / (fadeDuration * 1000.0f)) * dt;

			if (fadeState == FadeState::FADE_OUT)
			{
				fadeAlpha += fadeSpeed;
				if (fadeAlpha >= 255.0f)
				{
					fadeAlpha = 255.0f;
					fadeState = FadeState::FADE_IN;

					if (pendingMapChange) {

						std::string mapFile = nextMapName;
						if (mapFile.find(".tmx") == std::string::npos) {
							mapFile += ".tmx";
						}

						Engine::GetInstance().entityManager->ClearNonPlayerEntities();
						Engine::GetInstance().map->CleanUp();

						LoadLevel(mapFile);

						if (shouldMovePlayer && player != nullptr) {
							player->SetPosition(Vector2D(nextPlayerX, nextPlayerY));
							player->SetRespawnPosition(Vector2D(nextPlayerX, nextPlayerY));
						}

						pendingMapChange = false;
					}
					else if (pendingSceneChange) {
						ChangeScene(nextSceneId);

						if (isContinuing)
						{
							pugi::xml_document saveDoc;
							if (saveDoc.load_file("Saves/savegame.xml"))
							{
								pugi::xml_node root = saveDoc.child("save_estate");
								LoadGame(root);
							}

						}
						pendingSceneChange = false;
					}
				}
			}
			else if (fadeState == FadeState::FADE_IN)
			{
				fadeAlpha -= fadeSpeed;
				if (fadeAlpha <= 0.0f)
				{
					fadeAlpha = 0.0f;
					fadeState = FadeState::NONE;

					Engine::GetInstance().paused = false;
				}
			}

			int w, h;
			Engine::GetInstance().window->GetWindowSize(w, h);
			SDL_Rect screenRect = { 0, 0, w, h };

			Engine::GetInstance().render->DrawRectangle(screenRect, 0, 0, 0, (Uint8)fadeAlpha, true, false);
		}
	}

	if (Engine::GetInstance().quit == true)
		ret = false;

	return ret;
}

bool Scene::OnUIMouseClickEvent(UIElement* uiElement)
{
	Engine::GetInstance().audio->PlayFx(clicBotoFXId);
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
	if (boss != nullptr) boss->CleanUp();
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
	bool ret = true;

	auto& engine = Engine::GetInstance();

	switch (newScene)
	{
	case SceneID::MAIN_MENU:
		LoadMainMenu();
		break;

	case SceneID::LEVEL:

		if (isContinuing)
		{
			pugi::xml_document saveFile;
			pugi::xml_parse_result result = saveFile.load_file("Saves/savegame.xml");

			if (result)
			{
				std::string savedMapName = saveFile.child("save_estate").attribute("current_map").as_string();

				if (!savedMapName.empty())
				{
					LoadLevel(savedMapName + ".tmx");
				}
				else
				{
					LoadLevel("MapTemplate.tmx");
				}
			}
			else
			{
				LoadLevel("MapTemplate.tmx");
			}
		}
		else
		{
			//LoadLevel("MapTemplate.tmx");
			//PlayVideo("LoadingScreen");
			LoadLevel("MapTemplate.tmx");
			//LoadingScreenThread();
		}		

		
		

		//Create bosses when booting up to avoid lagging afterwards.
		/*std::shared_ptr<Entity> b1 = Engine::GetInstance().entityManager->CreateEntity(EntityType::BOSS1);
		boss = std::dynamic_pointer_cast<Boss1>(b1);
		
		boss->position = Vector2D(0, 0);
		boss->Awake();
		boss->Start();*/

		std::shared_ptr<Entity> b2 = Engine::GetInstance().entityManager->CreateEntity(EntityType::BOSS2);
		boss2 = std::dynamic_pointer_cast<Boss2>(b2);

		boss2->position = Vector2D(0, 0);
		boss2->Awake();
		boss2->Start();

		//WaitLoadingLevel();
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
	if (boss != nullptr) {
		boss->CleanUp();
	}
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
}

// *********************************************
// PAUSE MENU functions
// *********************************************

void Scene::HandlePause() {
	if (isGameOver) return;

	if (fadeState != FadeState::NONE) return;

	bool escPressed = Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || Engine::GetInstance().input->GetControllerKey(SDL_GAMEPAD_BUTTON_START) == KEY_DOWN;
	bool tabPressed = Engine::GetInstance().input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN;

	UIManager::PauseMenuState uiState = Engine::GetInstance().uiManager->currentPauseState;
	bool inAnyInventoryTab = (uiState == UIManager::PauseMenuState::INVENTORY || uiState == UIManager::PauseMenuState::INVENTORY2 || uiState == UIManager::PauseMenuState::MINIMAP || uiState == UIManager::PauseMenuState::POWERUPS);

	if (!Engine::GetInstance().paused)
	{
		if (escPressed)
		{
			Engine::GetInstance().paused = true;
			currentPauseState = PauseMenuState::MAIN;
			Engine::GetInstance().uiManager->currentPauseState = UIManager::PauseMenuState::MAIN;

			Engine::GetInstance().uiManager->LoadPauseMenu();
		}
		else if (tabPressed)
		{
			Engine::GetInstance().audio->PlayFx(clicBotoFXId);
			Engine::GetInstance().paused = true;
			Engine::GetInstance().uiManager->LoadLastActiveTab();
		}
	}
	else
	{
		if (inAnyInventoryTab)
		{
			if (escPressed || tabPressed)
			{
				Engine::GetInstance().audio->PlayFx(clicBotoFXId);
				Engine::GetInstance().paused = false;
				currentPauseState = PauseMenuState::NOTPAUSED;
				Engine::GetInstance().uiManager->currentPauseState = UIManager::PauseMenuState::NOTPAUSED;

				Engine::GetInstance().uiManager->CleanUp();
			}
		}
		else
		{
			if (escPressed)
			{
				Engine::GetInstance().paused = false;
				currentPauseState = PauseMenuState::NOTPAUSED;
				Engine::GetInstance().uiManager->currentPauseState = UIManager::PauseMenuState::NOTPAUSED;

				Engine::GetInstance().uiManager->CleanUp();
			}
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

	Engine::GetInstance().audio->StopMusicFx();

	if (level == "MapTemplate.tmx")
	{

		Engine::GetInstance().audio->PlayMusicFx(fondoBocaFXId, 50);
	}
	else if (level.find("MapPulmo") != std::string::npos)
	{

		Engine::GetInstance().audio->PlayMusicFx(fondoPulmonesFXId, 50);
	}
	else if (level.find("MapCor") != std::string::npos)
	{

		Engine::GetInstance().audio->PlayFx(latidosFXId, 50);
	}
	else if (level.find("MapEstomac") != std::string::npos)
	{

		Engine::GetInstance().audio->PlayFx(fondoEstomagoFXId, 50);
	}
	else if (level.find("MapMedula") != std::string::npos)
	{

		Engine::GetInstance().audio->PlayFx(fondoMedulaFXId, 50);
	}

	std::string map = level;
	Engine::GetInstance().map->Load("Assets/Maps/", map);

	//Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(player);

	player->visitedRooms.insert(map);

	if (playerX && playerY) {
		player->SetPosition(Vector2D(playerX, playerY));
		player->position = Vector2D(playerX, playerY);
	}
}

void Scene::UpdateLevel(float dt) {

	HandlePause();
	if (Engine::GetInstance().paused && fadeState == FadeState::NONE) {
		Engine::GetInstance().map->DrawForeground();
		Engine::GetInstance().uiManager->ShowPauseMenu();
	}
	else if (Engine::GetInstance().paused && fadeState != FadeState::NONE)
	{
		Engine::GetInstance().map->DrawForeground();
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
	if (isPlayingVideo) return;
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
		pugi::xml_document saveDoc;
		saveDoc.load_file("Saves/savegame.xml");
		pugi::xml_node root = saveDoc.child("save_estate");
		LoadGame(root);
	}

	//L15 TODO 4: Call the function to save entities from the map
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) 
	{
		SaveGame();
	}
}

void Scene::ActivateGameOver()
{
	Engine::GetInstance().uiManager->ShowDeathScreen();
	Engine::GetInstance().entityManager->CleanUp(true);

	destroyedEntitiesIds.clear();
}

// *********************************************
// Video rendering functions
// *********************************************

void Scene::OnVideoFrame(plm_t* mpeg, plm_frame_t* frame, void* user)
{
	VideoData* vd = static_cast<VideoData*>(user);

	if (vd->buffer) 
	{
		LOG("Decoding video frame");
		plm_frame_to_rgba(frame, vd->buffer, vd->width * 4);
	}
}

void Scene::OnAudioFrame(plm_t* mpeg, plm_samples_t* samples, void* user)
{
	Uint32 dataSize = samples->count * sizeof(float) * 2;
	SDL_PutAudioStreamData(Engine::GetInstance().audio->music_stream_, samples->interleaved, dataSize);
}

void Scene::LoadVideo(VideoData* video, std::string _file) 
{
	std::string path = "Assets/Video/" + _file + ".mpg";
	const char* charPath = path.c_str();
	video->plm = plm_create_with_filename(charPath);

	if (!video->plm) 
	{
		LOG("ERROR: Could not find or open video file: %s", charPath);
		isPlayingVideo = false;
		return;
	}

	video->width = plm_get_width(video->plm);
	video->height = plm_get_height(video->plm);

	if (video->width == 0 || video->height == 0) 
	{
		LOG("ERROR: File %s is not a valid MPEG video.", charPath);
		plm_destroy(video->plm);
		video->plm = nullptr;
		isPlayingVideo = false;
		return;
	}

	LOG("Video loaded successfully: %s", charPath);

	plm_set_audio_enabled(video->plm, 1);
	video->file = _file;
	plm_set_loop(video->plm, 0);

	video->buffer = new uint8_t[video->width * video->height * 4];
	video->texture = SDL_CreateTexture(Engine::GetInstance().render->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, video->width, video->height);

	plm_set_video_decode_callback(video->plm, OnVideoFrame, video);
	plm_set_audio_decode_callback(video->plm, OnAudioFrame, video);

	if (_file == "AnimaticaIntro") video->duration = 30;
	if (_file == "AnimCaida") video->duration = 45;
	if (_file == "AnimaticaEnd") video->duration = 37;
	if (_file == "AnimaticaEndv2") video->duration = 37;
	if (_file == "LoadingScreen") video->duration = 13;

		

	videos.push_back(*video);
}

void Scene::PlayVideo(std::string _file)
{
	for (auto video : videos)
	{
		if (video.file == _file)
		{
			currentVideo = video;
			isPlayingVideo = true;
			Engine::GetInstance().uiManager->CleanUp();
			videoTimer.Start();
		}
	}
}

void Scene::StopCurrentVideo() {
	isPlayingVideo = false;

	if (currentVideo.plm) plm_destroy(currentVideo.plm);
	if (currentVideo.texture) SDL_DestroyTexture(currentVideo.texture);
	if (currentVideo.buffer) delete[] currentVideo.buffer;

	currentVideo.plm = nullptr;
	currentVideo.texture = nullptr;
	currentVideo.buffer = nullptr;

	if (currentVideo.file == "AnimCaida") fallingVideo = currentVideo;
	else if (currentVideo.file == "AnimaticaIntro") introVideo = currentVideo;
	else if (currentVideo.file == "LoadingScreen") loadingVideo = currentVideo;
	else if (currentVideo.file == "AnimaticaEnd") finalVideo = currentVideo;
	else if (currentVideo.file == "AnimaticaEndv2") finalVideov2 = currentVideo;
}

void Scene::TriggerFallingVideo(std::string mapName, float destX, float destY)
{
	if (fallingVideo.plm == nullptr)
	{
		LoadVideo(&fallingVideo, "AnimCaida");
	}

	if (fallingVideo.plm == nullptr) {
		StartFadeToMap(mapName, destX, destY);
		return;
	}

	if (player != nullptr)
	{
		player->lock = true;
	}

	currentVideo = fallingVideo;
	isPlayingVideo = true; 
	transitionAfterVideo = true;

	videoNextMap = mapName;
	videoNextX = destX;
	videoNextY = destY;
}

// *********************************************
// Fade transition functions
// *********************************************

void Scene::StartFadeToMap(std::string mapName, float targetX, float targetY, float duration)
{
	if (fadeState == FadeState::NONE)
	{
		fadeState = FadeState::FADE_OUT;
		fadeAlpha = 0.0f;
		fadeDuration = duration;
		pendingMapChange = true;
		nextMapName = mapName;

		Engine::GetInstance().paused = true;

		if (targetX != -1.0f && targetY != -1.0f) {
			nextPlayerX = targetX;
			nextPlayerY = targetY;
			shouldMovePlayer = true;
		}
		else {
			shouldMovePlayer = false;
		}
	}
}

void Scene::StartFadeToScene(SceneID sceneId, float duration)
{
	if (fadeState == FadeState::NONE)
	{
		fadeState = FadeState::FADE_OUT;
		fadeAlpha = 0.0f;
		fadeDuration = duration;
		pendingSceneChange = true;
		nextSceneId = sceneId;

		Engine::GetInstance().paused = true;
	}
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

		pugi::xml_node collectiblesNode = playerNode.append_child("collectibles");
		for (int c_num : player->list_collectibles)
		{
			pugi::xml_node itemNode = collectiblesNode.append_child("item");
			itemNode.append_attribute("c_num").set_value(c_num);
		}

		pugi::xml_node visitedRoomsNode = playerNode.append_child("visited_rooms");
		for (const std::string& room : player->visitedRooms)
		{
			pugi::xml_node roomNode = visitedRoomsNode.append_child("room");
			roomNode.append_attribute("name").set_value(room.c_str());
		}
	}

	pugi::xml_node worldNode = root.child("world");
	if (!worldNode) worldNode = root.append_child("world");

	std::string currentMapName = Engine::GetInstance().map->mapFileName;

	size_t lastDot = currentMapName.find_last_of(".");
	if (lastDot != std::string::npos)
	{
		currentMapName = currentMapName.substr(0, lastDot);
	}

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

	root.append_attribute("current_map").set_value(currentMapName.c_str());
	saveDoc.save_file("Saves/savegame.xml");
	//LOG("Game successfully saved in Saves/savegame.xml");
}

bool Scene::LoadGame(pugi::xml_node& root)
{
	if (!root) return false;

	std::string savedMapName = root.attribute("current_map").as_string("MapTemplate");
	std::string mapFile = savedMapName + ".tmx";

	destroyedEntitiesIds.clear();
	pugi::xml_node levelNode = root.child("world").find_child_by_attribute("level", "name", savedMapName.c_str());
	if (levelNode)
	{
		pugi::xml_node destroyedNode = levelNode.child("entities");
		for (pugi::xml_node entNode = destroyedNode.child("entity"); entNode; entNode = entNode.next_sibling("entity"))
		{
			int deadId = entNode.attribute("id").as_int();
			destroyedEntitiesIds.push_back(deadId);
		}
	}

	Engine::GetInstance().entityManager->ClearNonPlayerEntities();
	Engine::GetInstance().map->CleanUp();
	LoadLevel(mapFile);

	pugi::xml_node playerNode = root.child("player");
	if (playerNode)
	{
		float px = playerNode.child("position").attribute("x").as_float();
		float py = playerNode.child("position").attribute("y").as_float();

		if (player != nullptr) {
			player->SetPosition(Vector2D(px, py));
			player->SetRespawnPosition(Vector2D(px, py));
			player->playerCurrentHp = playerNode.child("stats").attribute("currentHp").as_int();
			player->playerMaxHp = playerNode.child("stats").attribute("maxHp").as_int();
		}

		pugi::xml_node upgradesNode = playerNode.child("upgrades");
		if (upgradesNode && player != nullptr)
		{
			player->hasPowerJump = upgradesNode.attribute("hasPowerJump").as_bool();
		}

		if (player != nullptr)
		{
			player->list_collectibles.clear();
			player->visitedRooms.clear();

			pugi::xml_node collectiblesNode = playerNode.child("collectibles");
			if (collectiblesNode)
			{
				for (pugi::xml_node itemNode = collectiblesNode.child("item"); itemNode; itemNode = itemNode.next_sibling("item")) 
				{
					int c_num = itemNode.attribute("c_num").as_int();
					player->list_collectibles.push_back(c_num);
				}
			}

			pugi::xml_node visitedRoomsNode = playerNode.child("visited_rooms");
			if (visitedRoomsNode)
			{
				for (pugi::xml_node roomNode = visitedRoomsNode.child("room"); roomNode; roomNode = roomNode.next_sibling("room"))
				{
					std::string roomName = roomNode.attribute("name").as_string();
					player->visitedRooms.insert(roomName);
				}
			}
		}
	}

	if (levelNode)
	{
		pugi::xml_node checkpointNode = levelNode.child("active_checkpoint");
		if (checkpointNode)
		{
			int activeCpId = checkpointNode.attribute("id").as_int(-1);
			if (activeCpId != -1)
			{
				std::shared_ptr<Entity> cpEnt = Engine::GetInstance().entityManager->GetEntityByTiledId(activeCpId);
				if (cpEnt != nullptr && cpEnt->type == EntityType::CHECKPOINT)
				{
					Checkpoint* cp = static_cast<Checkpoint*>(cpEnt.get());

					for (Checkpoint* otherCp : Checkpoint::allCheckpoints)
					{
						if (otherCp != cp && otherCp->IsActive())
						{
							otherCp->SetActive(false, true);
						}
					}

					cp->SetActive(true, true);
				}
			}
		}
	}

	//LOG("Game successfully loaded from savegame.xml");

	return true;
}

// *********************************************
// Loading Screen functions
// *********************************************

static int LoadLevelThread(void* data) {

	Engine::GetInstance().audio->StopMusicFx();

	std::string* level = static_cast<std::string*>(data);
	if (*level == "MapTemplate.tmx")
	{
		Engine::GetInstance().audio->PlayMusicFx(Engine::GetInstance().scene->fondoBocaFXId, 50);
	}
	else if ((*level).find("MapPulmo") != std::string::npos)
	{
		Engine::GetInstance().audio->PlayMusicFx(Engine::GetInstance().scene->fondoPulmonesFXId, 50);
	}
	else if ((*level).find("MapCor") != std::string::npos)
	{
		Engine::GetInstance().audio->PlayFx(Engine::GetInstance().scene->latidosFXId, 50);
	}
	else if ((*level).find("MapEstomac") != std::string::npos)
	{
		Engine::GetInstance().audio->PlayFx(Engine::GetInstance().scene->fondoEstomagoFXId, 50);
	}
	else if ((*level).find("MapMedula") != std::string::npos)
	{
		Engine::GetInstance().audio->PlayFx(Engine::GetInstance().scene->fondoMedulaFXId, 50);
	}

	std::string map = *level;
	Engine::GetInstance().map->Load("Assets/Maps/", map);

	//Call the function to load entities from the map
	Engine::GetInstance().map->LoadEntities(Engine::GetInstance().scene->player);

	Engine::GetInstance().scene->player->visitedRooms.insert(map);

	/*if (playerX && playerY) {
		player->SetPosition(Vector2D(playerX, playerY));
		player->position = Vector2D(playerX, playerY);
	}*/

	return 1;
}

void Scene::StartLoadLevel(std::string level) {
	std::string* dynamicString = new std::string(level);
	loadingThread = SDL_CreateThread(LoadLevelThread, "LoadingLevelThread", dynamicString);
}

void Scene::WaitLoadingLevel() {
	SDL_WaitThread(loadingThread, NULL);
}

void Scene::LoadingScreenThread()
{
	//struct VideoData {
	//	plm_t* plm = nullptr;
	//	SDL_Texture* texture = nullptr;
	//	uint8_t* buffer = nullptr;
	//	int width = 0;
	//	int height = 0;
	//};

	//VideoData* video = new VideoData;
	//float dt;
	//double delta_time = 0.0;

	//auto OnVideoFrame = [](plm_t* self, plm_frame_t* frame, void* user)
	//	{
	//		VideoData* vd = static_cast<VideoData*>(user);

	//		if (vd->buffer)
	//		{
	//			//LOG("Decoding video frame");
	//			plm_frame_to_rgba(frame, vd->buffer, vd->width * 4);
	//		}
	//	};

	//std::string path = "Assets/Video/LoadingScreen.mpg";
	//const char* charPath = path.c_str();
	//video->plm = plm_create_with_filename(charPath);

	//if (!video->plm)
	//{
	//	LOG("ERROR: Could not find or open video file: %s", charPath);
	//	//isPlayingVideo = false;
	//	return;
	//}

	//video->width = plm_get_width(video->plm);
	//video->height = plm_get_height(video->plm);

	//if (video->width == 0 || video->height == 0)
	//{
	//	LOG("ERROR: File %s is not a valid MPEG video.", charPath);
	//	plm_destroy(video->plm);
	//	video->plm = nullptr;
	//	//isPlayingVideo = false;
	//	return;
	//}

	//LOG("Video loaded successfully: %s", charPath);

	//plm_set_audio_enabled(video->plm, 0);
	//plm_set_loop(video->plm, 0);

	//video->buffer = new uint8_t[video->width * video->height * 4];
	//video->texture = SDL_CreateTexture(Engine::GetInstance().render->renderer, SDL_GetWindowPixelFormat(Engine::GetInstance().window->GetWindow()), SDL_TEXTUREACCESS_STREAMING, video->width, video->height);

	//plm_set_video_decode_callback(video->plm, OnVideoFrame, video);

	//Uint64 last_time = SDL_GetTicksNS();
	//void* mPixels;
	//int pitch;

	//while (!loadingDone) {

	//	Uint64 current_time = SDL_GetTicksNS();
	//	dt = (double)(current_time - last_time) / 1000000000.0;
	//	last_time = current_time;

	//	plm_decode(video->plm, dt);	// pl_mpeg uses time in seconds, dt is in milliseconds

	//	if (video->texture && video->buffer) {
	//		SDL_RenderClear(Engine::GetInstance().render->renderer);
	//		SDL_UpdateTexture(video->texture, NULL, video->buffer, video->width * 4);
	//		SDL_RenderTexture(Engine::GetInstance().render->renderer, video->texture, NULL, NULL);
	//		SDL_RenderPresent(Engine::GetInstance().render->renderer);
	//	}

	//	if (plm_has_ended(video->plm)) loadingDone = true;
	//}

	////VideoLock = SDL_CreateMutex();
	////SDL_LockMutex(VideoLock);

	//if (video->plm) plm_destroy(video->plm);
	//if (video->texture) SDL_DestroyTexture(video->texture);
	//if (video->buffer) delete[] video->buffer;

	//video->plm = nullptr;
	//video->texture = nullptr;
	//video->buffer = nullptr;
	//video = nullptr;

	////SDL_UnlockMutex(VideoLock);
	////SDL_DestroyMutex(VideoLock);
}