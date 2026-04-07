#pragma once
#include "UIElement.h"

class UISlider : public UIElement
{
public:
    UISlider(int id, SDL_Rect bounds, const char* text);

    bool Update(float dt) override;

    int GetValue() const { return value; }
    void SetValue(int v);

private:
    int value = 50;
    SDL_Rect knobBounds;
};

