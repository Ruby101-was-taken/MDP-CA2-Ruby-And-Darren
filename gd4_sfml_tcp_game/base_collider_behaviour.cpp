#include "base_collider_behaviour.hpp"
#include "scene_node.hpp"

BaseColliderBehaviour::BaseColliderBehaviour(CollisionLayer layer)
    : layer_(layer)
{
}

void BaseColliderBehaviour::Start() {
    node_->SetCollisionLayer(layer_);
}

void BaseColliderBehaviour::BeginCollisionFrame() {
    currentCollisions_.clear();
}

void BaseColliderBehaviour::RegisterCollision(SceneNode* other) {
    currentCollisions_.insert(other);

    // Collision ENTER detection
    if (previousCollisions_.find(other) == previousCollisions_.end()) {
        node_->OnCollision(other); // call only once
    }
}

void BaseColliderBehaviour::EndCollisionFrame() {
    previousCollisions_ = currentCollisions_;
}

bool BaseColliderBehaviour::IsCollding(BaseColliderBehaviour* other) {
    auto intersection = GetWorldBounds().findIntersection(other->GetWorldBounds());
    if (intersection)
        return true;
    return false;
}