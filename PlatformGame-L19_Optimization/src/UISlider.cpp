#include "UISlider.h"
#include "Engine.h"
#include "Render.h"
#include "Audio.h"

UISlider::UISlider(int id, SDL_Rect bounds, const char* text) : UIElement(UIElementType::SLIDER, id)
{
    this->bounds = bounds;
    this->text = text;
	this->texture = nullptr;

    knobBounds = { bounds.x, bounds.y, bounds.h, bounds.h };

	float currentVolume = Engine::GetInstance().audio->GetMusicVolume();
	int initialValue = (int)(currentVolume * 100.0f);

    SetValue(initialValue);
}

void UISlider::SetValue(int v) {
    if (v < 0) v = 0; if (v > 100) v = 100;
    value = v;

    float percentage = (float)value / 100.0f;

    knobBounds.x = bounds.x + (int)(percentage * (bounds.w - knobBounds.w));
}

bool UISlider::Update(float dt) {
    if (state == UIElementState::DISABLED) return true;

    Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

    float scaleW = Engine::GetInstance().render->GetRenderScale().W;
    float scaleH = Engine::GetInstance().render->GetRenderScale().H;

    if (mousePos.getX() > (bounds.x * scaleW) && mousePos.getX() < (bounds.x * scaleW) + (bounds.w * scaleW) &&
        mousePos.getY() > (bounds.y * scaleH) && mousePos.getY() < (bounds.y * scaleH) + (bounds.h * scaleH)) {

        if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT ||
            Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
        {
            float mouseRelX = mousePos.getX() - (bounds.x * scaleW);
            float width = (float)(bounds.w * scaleW);

            int newValue = (int)((mouseRelX / width) * 100);
            SetValue(newValue);

            NotifyObserver(); 
        }
        else if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
        {
            NotifyObserver(); // Notificar al soltar
        }
    }    

    if (texture != nullptr)
    {   
		// Draws the bar first and then the handle from the same texture
        Engine::GetInstance().render->DrawTexture(texture, bounds.x, bounds.y, nullptr, 0.0f);
        Engine::GetInstance().render->DrawTexture(texture, knobBounds.x, knobBounds.y, nullptr, 0.0f);
    }
    else
    {
        Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 255, 255, true, false);

        int radius = knobBounds.w / 2;
        int centerX = knobBounds.x + radius;
        int centerY = knobBounds.y + (knobBounds.h / 2);

        Engine::GetInstance().render->DrawCircle(centerX, centerY, radius, 255, 0, 0, 255, false);
    }

    return true;
}