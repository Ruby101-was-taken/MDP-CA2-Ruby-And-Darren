#include "text_node_behaviour.hpp"

TextNodeBehaviour::TextNodeBehaviour(const FontHolder& fonts, std::string text, sf::Vector2f offset) :
	fonts_(fonts),
	text_(text),
	offset_(offset)
{
}

TextNodeBehaviour::~TextNodeBehaviour() {
	delete text_node_;
	text_node_ = nullptr;
}

void TextNodeBehaviour::Start() {

	std::unique_ptr<TextNode> text_node = std::make_unique<TextNode>(fonts_, text_);
	text_node_ = text_node.get();
	text_node_->setPosition(offset_);
	node_->AttachChild(std::move(text_node));
}

void TextNodeBehaviour::Update(sf::Time dt, CommandQueue& commands) {
}

void TextNodeBehaviour::SetText(std::string text) {
	text_node_->SetString(text);
}
