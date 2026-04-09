// Ruby White - D00255322 - Whole Class
#pragma once
#include <SFML/Graphics.hpp>
#include "input_types.hpp"
#include <map>

class InputManager {
private:
	InputManager();

	static void BindInput(InputTypes input, sf::Keyboard::Scancode scan_code);

public:
	static void SetUpInputs();

	static bool InputIsPressed(InputTypes input);

	static void SetFocused(bool focus);

	static void SetInputEnabled(bool enabled); // Darren Meidl - D00255479 - Added method to enable/disable input checks

private:
	static std::map<InputTypes, sf::Keyboard::Scancode> input_map_;

	static bool window_focused_;
	static bool input_enabled_; // Darren Meidl - D00255479 - Track whether input checks are enabled
};

