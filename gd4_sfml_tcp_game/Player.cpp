// Ruby White - D00255322
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
#include "utility.hpp"

Player::Player(const TextureHolder& textures, const FontHolder& fonts, float x, float y, PlayerType type, std::string name, sf::Color colour)
	: SceneNode(x, y) ,
	type_(type)
{
	SpriteBehaviour* sprite;
	switch (type) {
	case PlayerType::kPlayerOne:
		AddBehaviour(new SpriteBehaviour(textures.Get(TextureID::kPlayerOneSheet)));
		break;
	case PlayerType::kPlayerTwo:
		AddBehaviour(new SpriteBehaviour(textures.Get(TextureID::kPlayerTwoSheet)));
		break;
	case PlayerType::kOnlineLocalPlayer:
		sprite = new SpriteBehaviour(textures.Get(TextureID::kOnlinePlayerSheet));
		AddBehaviour(sprite);
		// using hsv to get better colours                    // saturation is made higher to keep colours vibrant, value is always kept at max
		sprite->ColourSprite(Utility::GetUserColourFromFile());
		sprite->BlitToSprite(textures.Get(TextureID::kOnlinePlayerEyesSheet));
		break;
	case PlayerType::kOnlineNetworkedPlayer: // D00255479 - Darren Meidl - use colour sent by host to colour player sprite, also blit eyes so networked players have them too
		sprite = new SpriteBehaviour(textures.Get(TextureID::kOnlinePlayerSheet));
		AddBehaviour(sprite);
		if (colour == sf::Color::Transparent) {
			// If colour is transparent, it means there was an issue reading the colour data from the packet, so we will use a default colour
			std::cout << "Warning: Received transparent colour for networked player, using default colour instead." << std::endl;
			colour = sf::Color::White; // Default to white if no valid colour was provided
		}
		sprite->ColourSprite(colour); // Colour should be assigned by host, if not; use default colour from file
		sprite->BlitToSprite(textures.Get(TextureID::kOnlinePlayerEyesSheet));
		break;
	default:
		AddBehaviour(new SpriteBehaviour(textures.Get(TextureID::kPlayerOneSheet)));
		break;
	}


	// Darren Meidl - D00255479
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
	anim->AddAnimation("fall", { {16, 16}, 1, sf::seconds(0.1f), true, 3 });
	AddBehaviour(anim);

	AddBehaviour(new BoxColliderBehaviour({ 16.f, 16.f }, CollisionLayer::kPlayer));

	AddBehaviour(new HealthBehaviour(20));
	AddBehaviour(new PlayerMovementBehaviour(FindAttachable<BoxColliderBehaviour>(), type_));

	AddBehaviour(new PlayerAnimationBehaviour(FindAttachable<PlayerMovementBehaviour>(), anim));


	AddBehaviour(new PlayerScoreManager());

	AddBehaviour(new TextNodeBehaviour(fonts, "0", {8, -10}));

	switch (type) {
	case PlayerType::kOnlineLocalPlayer:
		AddBehaviour(new TextNodeBehaviour(fonts, Utility::GetUserNameFromFile(), { 8, -22 }));
		break;
	case PlayerType::kOnlineNetworkedPlayer:
		AddBehaviour(new TextNodeBehaviour(fonts, name, { 8, -22 }));
		break;
	default:
		break;
	}

	name_ = name;
}

void Player::UpdateCurrent(sf::Time dt, CommandQueue& commands) {
}

bool Player::IsReal() {
	return type_ != PlayerType::kOnlineNetworkedPlayer;
}


int Player::GetScore() {
	return FindAttachable<PlayerScoreManager>()->GetScore();
}

void Player::IncrementScore() {
	FindAttachable<PlayerScoreManager>()->IncrementScore();
}
// Darren Meidl - D00255479
void Player::Destroy() {
	// Player node 2 seconds before being destroyed:
	FindAttachable<PlayerScoreManager>()->ResetScore(); // Drop all stars when player is destroyed	
	DeleteNode();
}

ReceiverCategories Player::GetCategoryEnum() const {
	switch (type_) {
	case PlayerType::kPlayerOne:
		return ReceiverCategories::kPlayerOne;
	case PlayerType::kPlayerTwo:
		return ReceiverCategories::kPlayerTwo;
	case PlayerType::kOnlineLocalPlayer:
		return ReceiverCategories::kOnlineLocalPlayer;
	case PlayerType::kOnlineNetworkedPlayer:
		return ReceiverCategories::kOnlineNetworkedPlayer;
	default:
		return ReceiverCategories::kNone;
	}
}

