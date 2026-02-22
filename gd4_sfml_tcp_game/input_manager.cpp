// Ruby White - D00255322 - Whole Class
#include "input_manager.hpp"

std::map<InputTypes, sf::Keyboard::Scancode> InputManager::input_map_;

void InputManager::BindInput(InputTypes input, sf::Keyboard::Scancode scan_code) {
    input_map_[input] = scan_code;
}

void InputManager::SetUpInputs() {
    input_map_ = std::map<InputTypes, sf::Keyboard::Scancode>();

    BindInput(InputTypes::kPlayerOneUp, sf::Keyboard::Scancode::W);
    BindInput(InputTypes::kPlayerOneDown, sf::Keyboard::Scancode::S);
    BindInput(InputTypes::kPlayerOneLeft, sf::Keyboard::Scancode::A);
    BindInput(InputTypes::kPlayerOneRight, sf::Keyboard::Scancode::D);

    BindInput(InputTypes::kPlayerTwoUp, sf::Keyboard::Scancode::Up);
    BindInput(InputTypes::kPlayerTwoDown, sf::Keyboard::Scancode::Down);
    BindInput(InputTypes::kPlayerTwoLeft, sf::Keyboard::Scancode::Left);
    BindInput(InputTypes::kPlayerTwoRight, sf::Keyboard::Scancode::Right);
}

bool InputManager::InputIsPressed(InputTypes input) {
    return sf::Keyboard::isKeyPressed(input_map_[input]);
}
