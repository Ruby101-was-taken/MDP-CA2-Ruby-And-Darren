#include "player_movement_behaviour.hpp"
#include "scene_node.hpp"
#include "player.hpp"
#include "input_manager.hpp"
#include <iostream>
#include "constants.hpp"
#include "utility.hpp"
#include "state.hpp"
#include "World.hpp"
#include "level.hpp"
#include "physic_body.hpp"
#include "health_behaviour.hpp"
#include "star.hpp"
#include "star_spawner.hpp"
#include "player_score_manager.hpp"

PlayerMovementBehaviour::PlayerMovementBehaviour(BoxColliderBehaviour* collider, PlayerType type) :
    PhysicBody(collider, 2.f, 0.2f, 0.075f, 1.5f, 0.1f),
    type_(type),
    jump_power_(6.5f),
    jump_held_(false),
    invincibility_time_(0.f),
    sprite_(nullptr),
    last_star_spawn(0,0)
{
    random_number = 0;
    timer = 0.f;
}

void PlayerMovementBehaviour::Start() {
    sprite_ = node_->FindAttachable<SpriteBehaviour>();

    ReceiverCategories category = node_->GetCategoryEnum();

    get_score_.category = static_cast<int>(category);
    get_score_.action = DerivedAction<Player>([](Player& player, sf::Time) {
        PlayerScoreManager* score_manager = player.FindAttachable<PlayerScoreManager>();
        score_manager->IncrementScore();
        });

    lose_score_.category = static_cast<int>(category);
    lose_score_.action = DerivedAction<Player>([](Player& player, sf::Time) {
        PlayerScoreManager* score_manager = player.FindAttachable<PlayerScoreManager>();
        score_manager->DecrementScore();
        });
}




void PlayerMovementBehaviour::OnCollision(SceneNode* other) {
    if (other->GetCollisionLayer() == CollisionLayer::kPlayer) {
        PlayerMovementBehaviour* other_player = dynamic_cast<Player*>(other)->FindAttachable<PlayerMovementBehaviour>(); //get other player's component
        if (other_player->CanBeHit()) {
            if (other_player->velocity_.y < velocity_.y) { // hit the other player
                other_player->BouncePlayer(true);
                BouncePlayer(false);                
                other_player->MakeInvincible(2.f);
                MakeInvincible(0.01f);
                PlayLocalSound(node_->GetWorld()->GetCommandQueue(), SoundEffect::kPlayerCollide);
                std::cout << "I hit the other player!" << std::endl;               
            }
            else if (other_player->velocity_.y == velocity_.y and other_player->CanBeHit() and CanBeHit()) { // both player hit eachother
                other_player->BouncePlayer(true);
                BouncePlayer(true);
                other_player->MakeInvincible(2.f);
                MakeInvincible(2.f);
                PlayLocalSound(node_->GetWorld()->GetCommandQueue(), SoundEffect::kPlayerCollide);
            }
        }
    }
    else if (other->GetCollisionLayer() == CollisionLayer::kItemStar) {

        Star* star = dynamic_cast<Star*>(other); //get star

        if (star->CanBeCollected()) {
            std::cout << "get star!!!" << std::endl;
            star->Collect();
            PlayLocalSound(node_->GetWorld()->GetCommandQueue(), SoundEffect::kStarGet);
            node_->GetWorld()->GetCommandQueue().Push(get_score_);

            HealthBehaviour* star_health = star->FindAttachable<HealthBehaviour>(); //get star health
            if(star_health != nullptr)
                star_health->ChangeHealthBy(-1.f);
        }
    }
}


void PlayerMovementBehaviour::BouncePlayer(bool spawn_star) {
    velocity_.y = -velocity_.y - (GRAVITY * Utility::sign(velocity_.y, 10));
    if (spawn_star) {
        node_->GetWorld()->GetCommandQueue().Push(lose_score_);
    }
}


void PlayerMovementBehaviour::MakeInvincible(float time) {
    if (time > invincibility_time_)
        invincibility_time_ = time;
}

bool PlayerMovementBehaviour::CanBeHit() {
    return invincibility_time_ <= 0;
}

void PlayerMovementBehaviour::SetLastStarSpawn(sf::Vector2f pos) {
    last_star_spawn = pos;
}

void PlayerMovementBehaviour::SetLastPlayerPos(sf::Vector2f pos) {
    last_player_pos = pos;
}



void PlayerMovementBehaviour::PerformJump() {
    velocity_.y = -jump_power_;

    coyote_time_ = 0.f;
    jump_held_ = true;

    deceleration_speed_ = air_deceleration_speed_;

    PlayLocalSound(node_->GetWorld()->GetCommandQueue(), SoundEffect::kPlayerJump);
}

//returns true if this player can jump
bool PlayerMovementBehaviour::CanJump() {
    // if the player is yet to let go of jump then they can't jump unless they press it again
    if (jump_held_) return false;
    //if the player has c-time left they can always jump
    if (coyote_time_ > 0) return true;
    // if the player is on the ground you can always jump
    //if (IsOnGround()) return true; probs doesn't need to be checked since I don't think the above code covers all cases (c-time is always default on ground)
    return false; // <- for safety :3
}

// TODO: Update to be command queue
sf::Vector2f PlayerMovementBehaviour::HandlePlayerInput() {
    sf::Vector2f velocity(0.f, 0.f);

    if (timer <= 0) {
        timer = 0.5f;
        if (rand() % 2 == 0)
            random_number = (rand() % 100) - 50;
        else
            random_number = last_star_spawn.x - node_->GetWorldPosition().x;


    }
    if (rand() % 100 < 30) {
        if (CanJump())
            PerformJump();
    }
    else {
        jump_held_ = false;
    }
    if (random_number <= 0) {
        std::cout << random_number << std::endl;
        velocity.x -= 1;
    }
    if (random_number > 0) {
        std::cout << random_number << std::endl;
        velocity.x += 1;
    }

    return velocity;

    //creates the unit vector of movement
    if (type_ == PlayerType::kPlayerOne) {
        if (InputManager::InputIsPressed(InputTypes::kPlayerOneUp)) {
            if (CanJump())
                PerformJump();
        }
        else {
            jump_held_ = false;
        }
        if (InputManager::InputIsPressed(InputTypes::kPlayerOneLeft))
            velocity.x -= 1;
        if (InputManager::InputIsPressed(InputTypes::kPlayerOneRight))
            velocity.x += 1;
    }
    else if (type_ == PlayerType::kPlayerTwo) {
        if (InputManager::InputIsPressed(InputTypes::kPlayerTwoUp)) {
            if (CanJump())
                PerformJump();
        }
        else {
            jump_held_ = false;
        }
        if (InputManager::InputIsPressed(InputTypes::kPlayerTwoLeft))
            velocity.x -= 1;
        if (InputManager::InputIsPressed(InputTypes::kPlayerTwoRight))
            velocity.x += 1;
    }

    return velocity;
}

sf::Vector2f PlayerMovementBehaviour::CustomPhysicsUpdate(sf::Time dt, CommandQueue& commands) {

    timer -= dt.asSeconds();

    if (invincibility_time_ > 0) {
        sprite_->ToggleVisibilty();
        invincibility_time_ -= dt.asSeconds();
        if (CanBeHit())
            sprite_->Show();
    }

    if (velocity_.x != 0)
        sprite_->SetFlipX(velocity_.x < 0);

    return HandlePlayerInput() * acceleration_speed_;
}
