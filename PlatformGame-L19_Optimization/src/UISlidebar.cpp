#include "UISlidebar.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"

UISlidebar::UISlidebar(int id, SDL_Rect bounds, const char* text) : UIElement(UIElementType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;
}

UISlidebar::~UISlidebar()
{

}

bool UISlidebar::Update(float dt)
{
	
	return false;
}

bool UISlidebar::CleanUp()
{
	pendingToDelete = true;
	return true;
}