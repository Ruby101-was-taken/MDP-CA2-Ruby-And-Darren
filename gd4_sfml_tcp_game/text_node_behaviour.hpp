// Ruby White - D00255322 - Whole Class
#pragma once
#include "attachable_behaviour.hpp"
#include "text_node.hpp"

class TextNodeBehaviour : public AttachableBehaviour {
public:
	TextNodeBehaviour(const FontHolder& fonts, std::string text, sf::Vector2f offset = {0.f, 0.f});

	~TextNodeBehaviour();

	void Start() override;

	void Update(sf::Time dt, CommandQueue& commands) override;

	void SetText(std::string text);

private:
	TextNode* text_node_;
	const FontHolder& fonts_;
	sf::Vector2f offset_;
	std::string text_;
};

