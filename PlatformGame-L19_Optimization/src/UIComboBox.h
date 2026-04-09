#pragma once

#include "UIElement.h"
#include "Vector2D.h"

class UIComboBox : public UIElement
{

public:

	UIComboBox(int id, SDL_Rect bounds, const char* text);
	virtual ~UIComboBox();

	// Called each loop iteration
	bool Update(float dt);
	
	bool CleanUp() override;

private:

	bool canClick = true;
	bool drawBasic = false;
};

#pragma once