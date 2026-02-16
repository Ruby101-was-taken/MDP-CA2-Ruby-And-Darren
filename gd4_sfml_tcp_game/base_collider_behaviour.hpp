#pragma once
#include "attachable_behaviour.hpp"
#include "collision_layer.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <unordered_set>

class BaseColliderBehaviour : public AttachableBehaviour {
protected:
    std::unordered_set<SceneNode*> currentCollisions_;
    std::unordered_set<SceneNode*> previousCollisions_;
public:
    CollisionLayer layer_;
    bool is_trigger_;

    BaseColliderBehaviour(
        CollisionLayer layer,
        bool is_trigger = false
    );

    void Update(sf::Time, CommandQueue&) override {}
    void Start() override;
    virtual sf::FloatRect GetWorldBounds() const = 0;

    bool IsCollding(BaseColliderBehaviour* other);

    void BeginCollisionFrame();
    void RegisterCollision(SceneNode* other);
    void EndCollisionFrame();
};
