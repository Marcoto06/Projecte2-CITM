#include "UIManager.h"
#include "UIButton.h"
#include "UISlider.h"
#include "UICheckBox.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"

UIManager::UIManager() :Module()
{
	name = "UIManager";
}

UIManager::~UIManager() {}

bool UIManager::Start()
{
	return true;
}

std::shared_ptr<UIElement> UIManager::CreateUIElement(UIElementType type, int id, const char* text, SDL_Rect bounds, Module* observer, SDL_Rect sliderBounds)
{
	std::shared_ptr<UIElement> uiElement = std::make_shared<UIElement>();
	uiElement->type = type;
	// L16: TODO 1: Implement CreateUIElement function that instantiates a new UIElement according to the UIElementType and add it to the list of UIElements
	//Call the constructor according to the UIElementType
	switch (type)
	{
	case UIElementType::BUTTON:
		uiElement = std::make_shared<UIButton>(id, bounds, text);
		break;
	case UIElementType::SLIDER:
		uiElement = std::make_shared<UISlider>(id, bounds, text);
		break;
	case UIElementType::CHECKBOX: {
		uiElement = std::make_shared<UICheckBox>(id, bounds, text);
		if (text == " FULLSCREEN ") {
			bool value = Engine::GetInstance().render->IsFullScreen();
			uiElement = std::make_shared<UICheckBox>(id, bounds, text, value);
		}
		else {
			uiElement = std::make_shared<UICheckBox>(id, bounds, text);
		}
		break;
	}
	}
	
	//Set the observer
	uiElement->observer = observer;

	// Created GuiControls are add it to the list of controls
	UIElementsList.push_back(uiElement);

	return uiElement;
}

bool UIManager::Update(float dt)
{	
	std::vector<std::shared_ptr<UIElement>> elementsToUpdate;
	for (const auto& uiElement : UIElementsList)
	{
		elementsToUpdate.push_back(uiElement);
	}

	// 2. Actualizamos solo los elementos de nuestra copia segura
	for (const auto& uiElement : elementsToUpdate)
	{
		if (!uiElement->pendingToDelete)
		{
			uiElement->Update(dt);
		}
	}
	//List to store entities pending deletion
	std::list<std::shared_ptr<UIElement>> pendingDelete;

	for (const auto& uiElement : UIElementsList)
	{
		//If the entity is marked for deletion, add it to the pendingDelete list
		if (uiElement->pendingToDelete)
		{
			pendingDelete.push_back(uiElement);
		}
	}

	//Now iterates over the pendingDelete list and destroys the uiElement
	for (const auto uiElement : pendingDelete)
	{
		uiElement->CleanUp();
		UIElementsList.remove(uiElement);
	}

	return true;
}

bool UIManager::CleanUp()
{
	for (const auto& uiElement : UIElementsList)
	{
		uiElement->CleanUp();
	}

	UIElementsList.clear();

	return true;
}

std::shared_ptr<UIElement> UIManager::GetElement(int id) {
	for (const auto& uiElement : UIElementsList)
	{
		if (uiElement->id == id)
			return uiElement;
	}
}
