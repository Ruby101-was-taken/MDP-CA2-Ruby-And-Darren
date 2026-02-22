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

private:
	static std::map<InputTypes, sf::Keyboard::Scancode> input_map_;
};

