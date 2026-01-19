#include "player.hpp"
#include "receiver_categories.hpp"

struct AircraftMover
{
};

Player::Player(): current_mission_status_(MissionStatus::kMissionRunning)
{
    //Set initial key bindings
    key_binding_[sf::Keyboard::Key::A] = Action::kMoveLeft;
    key_binding_[sf::Keyboard::Key::D] = Action::kMoveRight;
    key_binding_[sf::Keyboard::Key::W] = Action::kMoveUp;
    key_binding_[sf::Keyboard::Key::S] = Action::kMoveDown;
    key_binding_[sf::Keyboard::Key::M] = Action::kMissileFire;
    key_binding_[sf::Keyboard::Key::Space] = Action::kBulletFire;

    //Set initial action bindings
    InitialiseActions();

    //Assign all categories to a player's aircraft
    for (auto& pair : action_binding_)
    {
        pair.second.category = static_cast<unsigned int>(ReceiverCategories::kPlayerAircraft);
    }
}

void Player::HandleEvent(const sf::Event& event, CommandQueue& command_queue)
{
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        auto found = key_binding_.find(keyPressed->code);
        if (found != key_binding_.end() && !IsRealTimeAction(found->second))
        {
            command_queue.Push(action_binding_[found->second]);
        }
    }
}

void Player::HandleRealTimeInput(CommandQueue& command_queue)
{
    //Check if any of the key bindings are pressed
    for (auto pair : key_binding_)
    {
        if (sf::Keyboard::isKeyPressed(pair.first) && IsRealTimeAction(pair.second))
        {
            command_queue.Push(action_binding_[pair.second]);
        }
    }
}

void Player::AssignKey(Action action, sf::Keyboard::Key key)
{
    //Remove keys that are currently bound to the action
    for (auto itr = key_binding_.begin(); itr != key_binding_.end();)
    {
        if (itr->second == action)
        {
            key_binding_.erase(itr++);
        }
        else
        {
            ++itr;
        }
    }
    key_binding_[key] = action;
}

sf::Keyboard::Key Player::GetAssignedKey(Action action) const
{
    for (auto pair : key_binding_)
    {
        if (pair.second == action)
        {
            return pair.first;
        }
    }
    return sf::Keyboard::Key::Unknown;
}

void Player::SetMissionStatus(MissionStatus status)
{
    current_mission_status_ = status;
}

MissionStatus Player::GetMissionStatus() const
{
    return current_mission_status_;
}

void Player::InitialiseActions()
{
    const float kPlayerSpeed = 200.f;
    action_binding_[Action::kMoveLeft].action = DerivedAction<Aircraft>(AircraftMover(-kPlayerSpeed, 0.f));
    action_binding_[Action::kMoveRight].action = DerivedAction<Aircraft>(AircraftMover(kPlayerSpeed, 0.f));
    action_binding_[Action::kMoveUp].action = DerivedAction<Aircraft>(AircraftMover(0.f, -kPlayerSpeed));
    action_binding_[Action::kMoveDown].action = DerivedAction<Aircraft>(AircraftMover(0.f, kPlayerSpeed));
    action_binding_[Action::kBulletFire].action = DerivedAction<Aircraft>([](Aircraft& a, sf::Time dt)
        {
            a.Fire();
        }
    );

    action_binding_[Action::kMissileFire].action = DerivedAction<Aircraft>([](Aircraft& a, sf::Time dt)
        {
            a.LaunchMissile();
        }
    );

}

bool Player::IsRealTimeAction(Action action)
{
    switch (action)
    {
    case Action::kMoveLeft:
    case Action::kMoveRight:
    case Action::kMoveDown:
    case Action::kMoveUp:
    case Action::kBulletFire:
        return true;
    default:
        return false;
    }
}
