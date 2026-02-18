#include "player.hpp"
#include "resource_holder.hpp"
#include "health_behaviour.hpp"
#include "sprite_behaviour.hpp"
#include "player_movement_behaviour.hpp"
#include "box_collider_behaviour.hpp"
#include "animation_behaviour.hpp"
#include <iostream>

Player::Player(const TextureHolder& textures, float x, float y, PlayerType type) 
	: SceneNode(x, y) ,
	type_(type)
{
	switch (type) {
	case PlayerType::kPlayerOne:
		AddBehaviour(new SpriteBehaviour(textures.Get(TextureID::kPlayerOneSheet)));
		break;
	case PlayerType::kPlayerTwo:
		AddBehaviour(new SpriteBehaviour(textures.Get(TextureID::kPlayerTwo)));
		break;
	default:
		AddBehaviour(new SpriteBehaviour(textures.Get(TextureID::kPlayerOneSheet)));
		break;
	}
	auto* anim = new AnimationBehaviour();

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
	AddBehaviour(new PlayerMovementBehaviour(type_));
	AddBehaviour(new BoxColliderBehaviour({ 16.f, 16.f }, CollisionLayer::kPlayer));
}

void Player::UpdateCurrent(sf::Time dt, CommandQueue& commands) {
	move(GetVelocity() * dt.asSeconds());

	// TODO: Seems expensive, store as variable instead? Find a better solution.
	auto* anim = FindAttachable<AnimationBehaviour>();
	auto* movement = FindAttachable<PlayerMovementBehaviour>();

	std::cout << movement->GetVelocity().y << std::endl;

	if (!anim) return;
	if (movement->GetVelocity().y != 0.f)
		anim->Play("jump");
	else if (movement->GetVelocity().x != 0.f)
		anim->Play("run");
	else
		anim->Play("idle");
}


