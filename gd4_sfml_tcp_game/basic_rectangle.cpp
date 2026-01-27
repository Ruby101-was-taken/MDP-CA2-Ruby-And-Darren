#include "basic_rectangle.hpp"
#include "box_collider_behaviour.hpp"
#include "sprite_behaviour.hpp"

BasicRectangle::BasicRectangle(unsigned int w, unsigned int h) :
	w_(w),
	h_(h)
{
	AddBehaviour(new BoxColliderBehaviour({ 32.f, 32.f }, CollisionLayer::kPlayer));
}


