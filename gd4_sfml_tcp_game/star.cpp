#include "star.hpp"
#include "sprite_behaviour.hpp"
#include "health_behaviour.hpp"
#include "box_collider_behaviour.hpp"
#include "World.hpp"

Star::Star(const TextureHolder& textures) {

	AddBehaviour(new SpriteBehaviour(textures.Get(TextureID::kItemStar)));
	AddBehaviour(new HealthBehaviour(1.f));
	AddBehaviour(new BoxColliderBehaviour({32.f, 32.f}, CollisionLayer::kItemStar));
	
	setPosition({200, 200});
}
