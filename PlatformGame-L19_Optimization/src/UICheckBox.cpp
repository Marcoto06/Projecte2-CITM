#include "UICheckBox.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"
#include "Textures.h"


UICheckBox::UICheckBox(int id, SDL_Rect bounds, const char* text) : UIElement(UIElementType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;
}

UICheckBox::UICheckBox(int id, SDL_Rect bounds, const char* text, bool val) : UIElement(UIElementType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;
	value = val;
}

UICheckBox::~UICheckBox()
{

}

bool UICheckBox::Update(float dt)
{
	if (state != UIElementState::DISABLED)
	{
		// L16: TODO 3: Update the state of the GUiButton according to the mouse position
		Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

		float scaleW = Engine::GetInstance().render->GetRenderScale().W;
		float scaleH = Engine::GetInstance().render->GetRenderScale().H;

		//If the position of the mouse if inside the bounds of the button 
		if (mousePos.getX() > (bounds.x * scaleW) && mousePos.getX() < (bounds.x * scaleW) + (bounds.w * scaleW) && 
			mousePos.getY() > (bounds.y * scaleH) && mousePos.getY() < (bounds.y * scaleH) + (bounds.h * scaleH)) {

			state = UIElementState::FOCUSED;

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				state = UIElementState::PRESSED;
			}

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				NotifyObserver();
			}
		}
		else {
			state = UIElementState::NORMAL;
		}

		//L16: TODO 4: Draw the button according the GuiControl State

		if (texture != nullptr)
		{
			int texW, texH;
			Engine::GetInstance().textures->GetSize(texture, texW, texH);

			int frameHeight = texH / 4;

			section.x = 0;
			section.w = texW;
			section.h = frameHeight;

			switch (state)
			{
			case UIElementState::DISABLED:
			case UIElementState::NORMAL:
				section.y = 0;
				break;
			case UIElementState::FOCUSED:
				section.y = frameHeight;
				break;
			case UIElementState::PRESSED:
				section.y = frameHeight * 2;
				break;
			case UIElementState::SELECTED:				
			default:
				section.y = 0;
				break;
			}

			Engine::GetInstance().render->DrawTexture(texture, bounds.x, bounds.y, &section, 0.0f);
		}
		else
		{
			switch (state)
			{
			case UIElementState::DISABLED:
				Engine::GetInstance().render->DrawRectangle(bounds, 200, 200, 200, 255, true, false);
				break;
			case UIElementState::NORMAL:
				Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 255, 255, true, false);
				break;
			case UIElementState::FOCUSED:
				Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 20, 255, true, false);
				break;
			case UIElementState::PRESSED:
				Engine::GetInstance().render->DrawRectangle(bounds, 0, 255, 0, 255, true, false);
				break;
			}

			if (value) {
				Engine::GetInstance().render->DrawText("X", bounds.x, bounds.y, 40, 40, {255,255,255,255});
			}
			
			Engine::GetInstance().render->DrawText(text.c_str(), bounds.x + 60, bounds.y, 160, 40, { 255,255,255,255 });
		}
	}

	return false;
}

bool UICheckBox::CleanUp()
{
	pendingToDelete = true;
	return true;
}

void UICheckBox::SetValue(bool val) {
	value = val;
}