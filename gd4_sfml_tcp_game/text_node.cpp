#include "text_node.hpp"
#include "resource_holder.hpp"
#include "utility.hpp"

TextNode::TextNode(const FontHolder& fonts, std::string& text)
	:text_(fonts.Get(Font::kMain))
{
	text_.setCharacterSize(20);
	SetString(text);
}

void TextNode::SetString(const std::string& text)
{

	text_.setString(text);
	Utility::CentreOrigin(text_);
}

void TextNode::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(text_, states);
}
