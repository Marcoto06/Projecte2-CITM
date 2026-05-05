#include "Engine.h"
#include "Input.h"
#include "Window.h"
#include "Log.h"

#define MAX_KEYS 300

int count = 0;



Input::Input() : Module()
{
	name = "input";
	controller = NULL;

	keyboard = new KeyState[MAX_KEYS];
	controllerButtons = new KeyState[MAX_KEYS];

	memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(controllerButtons, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouseButtons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
	memset(windowEvents, 0, sizeof(windowEvents));
	mouseMotionX = mouseMotionY = mouseX = mouseY = 0;
}

// Destructor
Input::~Input()
{
	delete[] keyboard;
	//delete[] controllerButtons;
}

// Called before render is available
bool Input::Awake()
{
	LOG("Init SDL input event system");
	bool ret = true;
	
	/* CONTROLLER INTEGRATION */
	SDL_SetHint(SDL_HINT_JOYSTICK_RAWINPUT, "0");

	if (SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMEPAD) != true)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called before the first frame
bool Input::Start()
{
	SDL_StopTextInput(Engine::GetInstance().window->window);
	return true;
}

// Called each loop iteration
bool Input::PreUpdate()
{
	static SDL_Event event;

	int numKeys = 0;
	const bool* keys = SDL_GetKeyboardState(&numKeys);
	controllerButtons = new KeyState[MAX_KEYS];

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;

			/*if (controllerButtons[i] == KEY_IDLE)
				controllerButtons[i] = KEY_DOWN;
			else
				controllerButtons[i] = KEY_REPEAT;*/
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;

			/*if (controllerButtons[i] == KEY_REPEAT || controllerButtons[i] == KEY_DOWN)
				controllerButtons[i] = KEY_UP;
			else
				controllerButtons[i] = KEY_IDLE;*/
		}
	}

	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if (mouseButtons[i] == KEY_DOWN)
			mouseButtons[i] = KEY_REPEAT;

		if (mouseButtons[i] == KEY_UP)
			mouseButtons[i] = KEY_IDLE;
	}

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_EVENT_QUIT:
			windowEvents[WE_QUIT] = true;
			break;

		case SDL_EVENT_WINDOW_HIDDEN:
		case SDL_EVENT_WINDOW_MINIMIZED:
		case SDL_EVENT_WINDOW_FOCUS_LOST:
			windowEvents[WE_HIDE] = true;
			break;

		case SDL_EVENT_WINDOW_SHOWN:
		case SDL_EVENT_WINDOW_FOCUS_GAINED:
		case SDL_EVENT_WINDOW_MAXIMIZED:
		case SDL_EVENT_WINDOW_RESTORED:
			windowEvents[WE_SHOW] = true;
			break;

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if (event.button.button >= 1 && event.button.button <= NUM_MOUSE_BUTTONS)
				mouseButtons[event.button.button - 1] = KEY_DOWN;
			break;

		case SDL_EVENT_MOUSE_BUTTON_UP:
			if (event.button.button >= 1 && event.button.button <= NUM_MOUSE_BUTTONS)
				mouseButtons[event.button.button - 1] = KEY_UP;
			break;

		case SDL_EVENT_MOUSE_MOTION:
		{
			int scale = Engine::GetInstance().window->GetScale();
			mouseMotionX = (int)(event.motion.xrel / scale);
			mouseMotionY = (int)(event.motion.yrel / scale);
			mouseX = (int)(event.motion.x / scale);
			mouseY = (int)(event.motion.y / scale);
			break;
		}
		case SDL_EVENT_GAMEPAD_ADDED:
			if (!controller) {
				CheckController();
			}
			break;
		case SDL_EVENT_GAMEPAD_REMOVED:
			SDL_CloseGamepad(controller);
			controller = NULL;
			break;
		case SDL_EVENT_GAMEPAD_BUTTON_DOWN: {
			if (event.gbutton.button >= 0)
				controllerButtons[event.gbutton.button - 1] = KEY_DOWN;
			break;
		}
		case SDL_EVENT_GAMEPAD_BUTTON_UP: {
			if (event.gbutton.button >= 0)
				controllerButtons[event.gbutton.button - 1] = KEY_UP;
			break;
		}
		case SDL_EVENT_GAMEPAD_AXIS_MOTION: {

			break;
		}
		}
	}

	return true;
}

// Called before quitting
bool Input::CleanUp()
{
	LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	SDL_CloseGamepad(controller);
	controller = NULL;

	return true;
}

bool Input::GetWindowEvent(EventWindow ev)
{
	return windowEvents[ev];
}

Vector2D Input::GetMousePosition()
{
	return Vector2D((float)mouseX,(float)mouseY);
}

Vector2D Input::GetMouseMotion()
{
	return Vector2D((float)mouseMotionX, (float)mouseMotionY);
}

bool Input::CheckController() {
	SDL_JoystickID* ids = SDL_GetGamepads(&count);
	for (int i = 0; i < count; i++) {
		SDL_Gamepad* gamepd = SDL_OpenGamepad(ids[i]);
		if (controller == NULL) {
			controller = gamepd;
		}

		std::cout << "Gamepad connected: " << SDL_GetGamepadName(gamepd) << "\n";

		// Close the other gamepads
		if (i > 0) {
			SDL_CloseGamepad(gamepd);
		}
	}

	if (!controller) {
		std::cerr << "Failed to open controller: " << SDL_GetError() << "\n";
		SDL_Quit();
		return false;
	}

	return true;
}