#include "AnimatedTile.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"

AnimatedTile::AnimatedTile() : Entity(EntityType::ANIMATED_TILE)
{
	name = "animated tile";
}

AnimatedTile::~AnimatedTile() {}

bool AnimatedTile::Awake() {
	return true;
}

bool AnimatedTile::Start() {
	return true;
}

bool AnimatedTile::Update(float dt)
{
	if (draw)
	{
		anims.Update(dt);
		const SDL_Rect& animFrame = anims.GetCurrentFrame();
		float texW = animFrame.w;
		float texH = animFrame.h;

		Engine::GetInstance().render->DrawTexture(texture, position.getX(), position.getY(), &animFrame, 1.0f, 0.0, texW, texH, SDL_FLIP_NONE, 1.0f);
		return true;
	}
}

bool AnimatedTile::CleanUp()
{
	return true;
}

bool AnimatedTile::Destroy()
{
	LOG("Destroying Tile");
	pendingToDelete = true;
	return true;
}

void AnimatedTile::LoadTexture(std::string _texture)
{
	std::string pngPath = ("Assets/Textures/AnimatedTiles/" + _texture + ".png");
	std::string tsxPath = ("Assets/Textures/AnimatedTiles/" + _texture + ".tsx");
	const char* pngPathChar = pngPath.c_str();
	const char* tsxPathChar = tsxPath.c_str();
	texture = Engine::GetInstance().textures->Load(pngPathChar);
	std::unordered_map<int, std::string> aliases = { {0,"idle"} };
	anims.LoadFromTSX(tsxPathChar, aliases);
	anims.SetCurrent("idle");
	draw = true;
}