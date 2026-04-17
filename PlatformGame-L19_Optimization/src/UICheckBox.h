#pragma once

#include "UIElement.h"
#include "Vector2D.h"

class UICheckBox : public UIElement
{

public:

	UICheckBox(int id, SDL_Rect bounds, const char* text);
	UICheckBox(int id, SDL_Rect bounds, const char* text, bool val);
	virtual ~UICheckBox();

	// Called each loop iteration
	bool Update(float dt);

	bool CleanUp() override;
	bool GetValue() const { return value; }
	void SetValue(bool val);

private:

	bool value = false;
	bool canClick = true;
	bool drawBasic = false;
};

#pragma once