#pragma once

#include "UIElement.h"
#include "Vector2D.h"

class UISlidebar : public UIElement
{

public:

	UISlidebar(int id, SDL_Rect bounds, const char* text);
	virtual ~UISlidebar();

	// Called each loop iteration
	bool Update(float dt);

	bool CleanUp() override;

private:

	bool canClick = true;
	bool drawBasic = false;
};

#pragma once