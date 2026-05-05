#pragma once
#include "UIElement.h"
#include "Textures.h"

class UISlider : public UIElement
{
public:
    UISlider(int id, SDL_Rect bounds, const char* text);

    bool Update(float dt) override;

    int GetValue() const { return value; }
    void SetValue(int v);
    int GetValue();

    void SetKnobTexture(SDL_Texture* tex);

private:
    int value = 50;
    SDL_Rect knobBounds;
    SDL_Texture* knobTexture = nullptr;
};

