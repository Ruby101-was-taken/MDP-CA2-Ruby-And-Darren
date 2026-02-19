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

PlayerMovementBehaviour::PlayerMovementBehaviour(PlayerType type) :
    acceleration_speed_(2.f),
    deceleration_speed_(0.2f),
    ground_deceleration_speed_(0.2f),
    air_deceleration_speed_(0.075f),
    type_(type),
    max_speed_(1.5f),
    can_play_collision_sound_(false),
    velocity_({ 0,0 }),
    jump_power_(6.5f),
    jump_held_(false),
    coyote_time_(0.f),
    default_coyote_time_(0.1f),
    invincibility_time_(0.f)
{
}

void PlayerMovementBehaviour::Start() {
    player_collider_ = node_->FindAttachable<BoxColliderBehaviour>();
    sprite_ = node_->FindAttachable<SpriteBehaviour>();
}

void PlayerMovementBehaviour::Update(sf::Time dt, CommandQueue& commands) {
    velocity_ += HandlePlayerInput()*acceleration_speed_;
    
    if (velocity_.x != 0)
        sprite_->SetFlipX(velocity_.x < 0);

    PerformGravity();

    //player is moving left or right
    int8_t direction = Utility::sign(velocity_.x);
    
    // moves the player
    velocity_ = { MoveInDirection(velocity_.x, {1, 0}), MoveInDirection(velocity_.y, {0, 1}) };
    PerformDeceleration();
    
    // reduce c-time when moving down and having c time
    if (coyote_time_ > 0 and velocity_.y > 0) {
        coyote_time_ -= dt.asSeconds();
        deceleration_speed_ = air_deceleration_speed_;
    }
    else if (coyote_time_ < default_coyote_time_ and velocity_.y == 0) {
        if (IsOnGround()) { // check on new line for performance
            coyote_time_ = default_coyote_time_;
            deceleration_speed_ = ground_deceleration_speed_;
        }
    }

    HandleSounds(commands);

    if (invincibility_time_ > 0) {
        sprite_->ToggleVisibilty();
        invincibility_time_ -= dt.asSeconds();
        if (CanBeHit())
            sprite_->Show();
    }
}



void PlayerMovementBehaviour::HandleSounds(CommandQueue& commands) {
    
    if (can_play_collision_sound_) {
        PlayLocalSound(commands, SoundEffect::kExplosion1);
        can_play_collision_sound_ = false;
    }

}


void PlayerMovementBehaviour::OnCollision(SceneNode* other) {
    if (other->GetCollisionLayer() == CollisionLayer::kPlayer) {
        PlayerMovementBehaviour* other_player = dynamic_cast<Player*>(other)->FindAttachable<PlayerMovementBehaviour>(); //get other player's component
        if (other_player->CanBeHit()) {
            if (other_player->velocity_.y < velocity_.y) {
                other_player->BouncePlayer();
                BouncePlayer();
                other_player->MakeInvincible(2.f);
                MakeInvincible(0.01f);
                PlayLocalSound(node_->GetWorld()->GetCommandQueue(), SoundEffect::kPlayerCollide);
            }
            else if (other_player->velocity_.y == velocity_.y) {
                other_player->BouncePlayer();
                BouncePlayer();
                other_player->MakeInvincible(2.f);
                MakeInvincible(2.f);
                PlayLocalSound(node_->GetWorld()->GetCommandQueue(), SoundEffect::kPlayerCollide);
            }
        }
    }
}

void PlayerMovementBehaviour::BouncePlayer() {
    velocity_.y = -velocity_.y - (GRAVITY * Utility::sign(velocity_.y, 10));
}

sf::Vector2f& PlayerMovementBehaviour::GetVelocity() {
    return velocity_;
}

void PlayerMovementBehaviour::MakeInvincible(float time) {
    if (time > invincibility_time_)
        invincibility_time_ = time;
}

bool PlayerMovementBehaviour::CanBeHit() {
    return invincibility_time_ <= 0;
}

void PlayerMovementBehaviour::PerformGravity() {
    velocity_.y += GRAVITY;
}

// moves the node in direction (e.g {1, 0} would be right) going at speed, speed can be neg (neg speed with {1, 0} would move left)
float PlayerMovementBehaviour::MoveInDirection(float speed, sf::Vector2f direction) {
    float return_speed = speed; // this value is retuned so we know what the velocity should be
    if (speed != 0) {
        node_->move(speed*direction);
        if (Level::IsCollidingWithLevel(player_collider_)) {
            float backwards = -Utility::sign(speed);
            if (abs(speed) < 1)
                backwards *= abs(speed);
            return_speed = 0;
            bool touching_level = true;
            while (touching_level) {
                node_->move(backwards*direction);
                touching_level = Level::IsCollidingWithLevel(player_collider_);
            }
        }
    }
    return return_speed;
}

void PlayerMovementBehaviour::PerformDeceleration() {
    int dir = Utility::sign(velocity_.x);
    velocity_.x -= deceleration_speed_*dir;
    if (Utility::sign(velocity_.x) != dir) {// we know the player change direction from decel
        velocity_.x = 0;
    }
    if (abs(velocity_.x) > max_speed_)
        velocity_.x = dir * max_speed_;
}

bool PlayerMovementBehaviour::IsOnGround() {
    // move player down and check if they touch ground
    node_->move({ 0, 1 });
    bool on_ground = Level::IsCollidingWithLevel(player_collider_);
    node_->move({ 0, -1 });
    return on_ground;
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
