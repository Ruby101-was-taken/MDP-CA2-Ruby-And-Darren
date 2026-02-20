#include "player.hpp"
#include "resource_holder.hpp"
#include "health_behaviour.hpp"
#include "sprite_behaviour.hpp"
#include "player_movement_behaviour.hpp"
#include "player_animation_behaviour.hpp"
#include "box_collider_behaviour.hpp"
#include "animation_behaviour.hpp"
#include "player_score_manager.hpp"
#include <iostream>
#include "star_spawner.hpp"
#include "text_node_behaviour.hpp"

Player::Player(const TextureHolder& textures, const FontHolder& fonts, float x, float y, PlayerType type)
	: SceneNode(x, y) ,
	type_(type)
{
	switch (type) {
	case PlayerType::kPlayerOne:
		AddBehaviour(new SpriteBehaviour(textures.Get(TextureID::kPlayerOneSheet)));
		break;
	case PlayerType::kPlayerTwo:
		AddBehaviour(new SpriteBehaviour(textures.Get(TextureID::kPlayerTwoSheet)));
		break;
	default:
		AddBehaviour(new SpriteBehaviour(textures.Get(TextureID::kPlayerOneSheet)));
		break;
	}
	AnimationBehaviour* anim = new AnimationBehaviour();

	anim->AddAnimation("idle", {
		{16, 16},          // frame size
		2,                 // frames
		sf::seconds(0.5f), // duration
		true,              // loop
		0                  // row in sheet
		});

	anim->AddAnimation("run", { {16, 16}, 3, sf::seconds(0.2f), true, 1 });
	anim->AddAnimation("jump", { {16, 16}, 3, sf::seconds(0.2f), false, 2 });
	AddBehaviour(anim);

	AddBehaviour(new HealthBehaviour(20));
	AddBehaviour(new BoxColliderBehaviour({ 16.f, 16.f }, CollisionLayer::kPlayer));
	AddBehaviour(new PlayerMovementBehaviour(FindAttachable<BoxColliderBehaviour>(), type_));

	AddBehaviour(new PlayerAnimationBehaviour(FindAttachable<PlayerMovementBehaviour>(), anim));


	AddBehaviour(new PlayerScoreManager());

	AddBehaviour(new TextNodeBehaviour(fonts, "0", {7, -10}));
}

void Player::UpdateCurrent(sf::Time dt, CommandQueue& commands) {
}


ReceiverCategories Player::GetCategoryEnum() const {
	ReceiverCategories category = (type_ == PlayerType::kPlayerOne) ? ReceiverCategories::kPlayerOne : ReceiverCategories::kPlayerTwo;

	return category;
}

