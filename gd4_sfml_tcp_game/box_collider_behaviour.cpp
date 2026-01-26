#include "box_collider_behaviour.hpp"
#include "scene_node.hpp"

BoxColliderBehaviour::BoxColliderBehaviour(sf::Vector2f size, CollisionLayer layer, bool is_trigger)
    : BaseColliderBehaviour(layer, is_trigger)
    , size_(size)
{
}

sf::FloatRect BoxColliderBehaviour::GetWorldBounds() const {
    sf::Vector2f pos = node_->GetWorldPosition();
    return sf::FloatRect({ pos.x, pos.y }, { size_.x, size_.y });
}
