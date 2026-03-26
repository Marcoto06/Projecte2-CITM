#include "UISlider.h"
#include "Engine.h"
#include "Render.h"

UISlider::UISlider(int id, SDL_Rect bounds, const char* text) : UIElement(UIElementType::SLIDER, id)
{
    this->bounds = bounds;
    this->text = text;

    knobBounds = { bounds.x, bounds.y, 20, bounds.h };
    SetValue(50);
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

    if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w &&
        mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h) {

        if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT ||
            Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
        {
            float mouseRelX = mousePos.getX() - bounds.x;
            float width = (float)bounds.w;

            int newValue = (int)((mouseRelX / width) * 100);
            SetValue(newValue);

            NotifyObserver(); 
        }
        else if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
        {
            NotifyObserver(); // Notificar al soltar
        }
    }    

    return true;
}

bool UISlider::Draw(float dt) 
{
    if (texture != nullptr)
    {
        Engine::GetInstance().render->DrawTexture(texture, knobBounds.x, knobBounds.y, nullptr, 0.0f);
    }
    else
    {
        Engine::GetInstance().render->DrawRectangle(knobBounds, 255, 0, 0, 255, true, false);
    }

	return true;
}
