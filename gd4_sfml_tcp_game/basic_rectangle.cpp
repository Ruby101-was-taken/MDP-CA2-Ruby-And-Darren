#include "basic_rectangle.hpp"
#include "box_collider_behaviour.hpp"
#include "sprite_behaviour.hpp"

BasicRectangle::BasicRectangle(unsigned int w, unsigned int h) :
	w_(w),
	h_(h)
{
	AddBehaviour(new BoxColliderBehaviour({ static_cast<float>(w), static_cast<float>(h) }, CollisionLayer::kWorld));
	sf::Image image;
	image.resize(sf::Vector2u( w, h ), sf::Color::Red); // fill whole image with one color

	sf::Texture* texture = new sf::Texture();
	texture->loadFromImage(image);

	//sf::Texture* texture = new sf::Texture("Media/Textures/Debug/Cube.png");

	AddBehaviour(new SpriteBehaviour(*texture));
}


