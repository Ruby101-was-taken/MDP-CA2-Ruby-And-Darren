#include "scene_node.hpp"
#include "utility.hpp"
#include <cassert>
#include <iostream>

SceneNode::SceneNode(float x, float y, ReceiverCategories category) 
    : children_(), 
    parent_(nullptr), 
    world_(nullptr),
    default_category_(category),
    collision_layer_(CollisionLayer::kNone)
{
    this->setPosition({ x, y });
}
SceneNode::~SceneNode() {
    for (AttachableBehaviour* behaviour : behaviours_) {
        delete behaviour;
    }
}


void SceneNode::AttachChild(Ptr child) {
    child->parent_ = this;
    child->SetWorld(world_);
    if (has_started_)
        child->Start();
    children_.emplace_back(std::move(child));
}

SceneNode::Ptr SceneNode::DetachChild(const SceneNode& node) {
    auto found = std::find_if(children_.begin(), children_.end(), [&](Ptr& p) {return p.get() == &node; });
    assert(found != children_.end());

    Ptr result = std::move(*found);
    result->parent_ = nullptr;
    children_.erase(found);
    return Ptr();
}

void SceneNode::SetWorld(World* world) {
    world_ = world;
}

World* SceneNode::GetWorld() {
    return world_;
}


sf::Vector2f SceneNode::GetWorldPosition() const
{
    return GetWorldTransform() * sf::Vector2f();
}

sf::Transform SceneNode::GetWorldTransform() const
{
    sf::Transform transform = sf::Transform::Identity;
    for (const SceneNode* node = this; node != nullptr; node = node->parent_)
    {
        transform = node->getTransform() * transform;
    }
    return transform;
}

void SceneNode::OnCommand(const Command& command, sf::Time dt)
{
    //Is this command for me? If it is execute
    //Regardless of answer forward to all of my children
    if (command.category & GetCategory())
    {
        command.action(*this, dt);
    }

    //Pass it on to my children
    for (Ptr& child : children_)
    {
        child->OnCommand(command, dt);
    }
}

sf::FloatRect SceneNode::GetBoundingRect() const
{
    return sf::FloatRect();
}

void SceneNode::DrawBoundingRect(sf::RenderTarget& target, sf::RenderStates states, sf::FloatRect& rect) const
{
    sf::RectangleShape shape;
    shape.setPosition(sf::Vector2f(rect.position.x, rect.position.y));
    shape.setSize(sf::Vector2f(rect.size.x, rect.size.y));
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineColor(sf::Color::Green);
    shape.setOutlineThickness(1.f);
    target.draw(shape);
}

bool Collision(const SceneNode& lhs, const SceneNode& rhs)
{
    return lhs.GetBoundingRect().findIntersection(rhs.GetBoundingRect()).has_value();
}

void SceneNode::RemoveWrecks()
{
    auto wreck_field_begin = std::remove_if(children_.begin(), children_.end(), std::mem_fn(&SceneNode::IsMarkedForRemoval));
    children_.erase(wreck_field_begin, children_.end());
    std::for_each(children_.begin(), children_.end(), std::mem_fn(&SceneNode::RemoveWrecks));
}

#pragma region StartAndUpdate

void SceneNode::Start() {
    if(!has_started_) {
        StartCurrent();
        StartAttachables();
        StartChildren();
        has_started_ = true;
    }
}

void SceneNode::StartAttachables() {
    for (AttachableBehaviour* behaviour : behaviours_) {
        behaviour->Start();
    }
}

void SceneNode::StartCurrent() {
    //Do nothing
}

void SceneNode::StartChildren() {
    for (Ptr& child : children_) {
        child->Start();
    }
}

void SceneNode::Update(sf::Time dt, CommandQueue& commands) {
    if (has_started_) {
        UpdateCurrent(dt, commands);
        UpdateAttachables(dt, commands);
        if (!isDeleted_)
            UpdateChildren(dt, commands);
    }
}

void SceneNode::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
    //Do nothing
}

void SceneNode::UpdateAttachables(sf::Time dt, CommandQueue& commands) {
    for (AttachableBehaviour* behaviour : behaviours_) {
        behaviour->Update(dt, commands);
    }
}

void SceneNode::UpdateChildren(sf::Time dt, CommandQueue& commands) {

    for (Ptr& child : children_) {
        child->Update(dt, commands);
    }
    

    children_.erase(
        std::remove_if(children_.begin(), children_.end(),
            [](const Ptr& child) {
                return child->isDeleted_;
            }),
        children_.end());
}

void SceneNode::OnCollision(SceneNode* other) {
    OnCollisionCurrent(other);
    OnCollisionAttachables(other);
}


void SceneNode::OnCollisionCurrent(SceneNode* other) {
    //Do nothing
}

void SceneNode::OnCollisionAttachables(SceneNode* other) {
    for (AttachableBehaviour* behaviour : behaviours_) {
        behaviour->OnCollision(other);
    }
}

#pragma endregion

void SceneNode::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    //Apply the tranform of the current node
    states.transform *= getTransform();
    //Draw the node and its children with the changed transform
    DrawCurrent(target, states);
    DrawChildren(target, states);
    sf::FloatRect rect = GetBoundingRect();
    DrawBoundingRect(target, states, rect);
}

void SceneNode::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (AttachableBehaviour* behaviour : behaviours_) {
        behaviour->Draw(target, states);
    }
}

void SceneNode::DrawChildren(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (const Ptr& child : children_)
    {
        child->draw(target, states);
    }
}

unsigned int SceneNode::GetCategory() const
{
    return static_cast<unsigned int>(GetCategoryEnum());
}

ReceiverCategories SceneNode::GetCategoryEnum() const {
    return default_category_;
}

void SceneNode::DeleteNode() {
    children_.clear();
    behaviours_.clear();
    // removes this element from the parent
    parent_ = nullptr;
    isDeleted_ = true;
}

bool SceneNode::IsDestroyed() const
{
    return isDeleted_;
}


void SceneNode::SetVelocity(sf::Vector2f v) {
    velocity_ = v;
}
void SceneNode::AddVelocity(sf::Vector2f v) {
    velocity_ += v;
}

sf::Vector2f SceneNode::GetVelocity() const {
    return velocity_;
}

bool SceneNode::IsMarkedForRemoval() const
{
    return IsDestroyed();
}

void SceneNode::SetCollisionLayer(CollisionLayer layer) {
    collision_layer_ = layer;
}

CollisionLayer SceneNode::GetCollisionLayer() {
    return collision_layer_;
}

void SceneNode::AddBehaviour(AttachableBehaviour* behaviour) {
    behaviour->SetupBehaviour(this);
    behaviours_.emplace_back(behaviour);

}
void SceneNode::CollectColliders(std::vector<BaseColliderBehaviour*>& colliders) {
    for (AttachableBehaviour* behaviour : behaviours_) {
        if (auto* collider = dynamic_cast<BaseColliderBehaviour*>(behaviour))
            colliders.emplace_back(collider);
    }

    for (const auto& child : children_) {
        child->CollectColliders(colliders);
    }
}

float Distance(const SceneNode& lhs, const SceneNode& rhs)
{
    return Utility::Length(lhs.GetWorldPosition() - rhs.GetWorldPosition());
}
