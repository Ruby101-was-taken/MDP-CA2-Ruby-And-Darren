#include "base_collider_behaviour.hpp"
#include "scene_node.hpp"

BaseColliderBehaviour::BaseColliderBehaviour(CollisionLayer layer, bool is_trigger)
    : layer_(layer)
    , is_trigger_(is_trigger)
{
}