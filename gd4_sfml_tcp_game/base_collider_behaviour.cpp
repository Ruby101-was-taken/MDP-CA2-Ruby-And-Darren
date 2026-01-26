#include "base_collider_behaviour.hpp"
#include "scene_node.hpp"

BaseColliderBehaviour::BaseColliderBehaviour(CollisionLayer layer_, bool is_trigger_)
    : layer(layer_)
    , is_trigger(is_trigger_)
{
}