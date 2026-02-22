#pragma once
#include <memory>
#include <SFML/Graphics.hpp>
#include "receiver_categories.hpp"
#include "command_queue.hpp"
#include "command.hpp"
#include "attachable_behaviour.hpp"
#include "base_collider_behaviour.hpp"

#include <set>
#include <vector>

class World; //forward declare to avoid cicrular references

class SceneNode : public sf::Transformable, public sf::Drawable {
public:
	typedef std::unique_ptr<SceneNode> Ptr;
	typedef std::pair<SceneNode*, SceneNode*> Pair;

public:
	SceneNode(float x=0, float y=0, ReceiverCategories cateogry = ReceiverCategories::kNone);

	void AttachChild(Ptr child);
	Ptr DetachChild(const SceneNode& node);
	
	void SetWorld(World* world);
	World* GetWorld();

	void Start();
	void Update(sf::Time dt, CommandQueue& commands);
	void OnCollision(SceneNode* other);

	sf::Vector2f GetWorldPosition() const;
	sf::Transform GetWorldTransform() const;

	void OnCommand(const Command& command, sf::Time dt);

	virtual sf::FloatRect GetBoundingRect() const;
	void DrawBoundingRect(sf::RenderTarget& target, sf::RenderStates states, sf::FloatRect& rect) const;
;
	void RemoveWrecks();
	unsigned int GetCategory() const;
	virtual ReceiverCategories GetCategoryEnum() const;

	void DeleteNode();

	bool IsDestroyed() const;

	void SetVelocity(sf::Vector2f v);
	void AddVelocity(sf::Vector2f v);
	sf::Vector2f GetVelocity() const;

	void CollectColliders(std::vector<BaseColliderBehaviour*>& colliders);

	void SetCollisionLayer(CollisionLayer layer);
	CollisionLayer GetCollisionLayer();

	template<typename Attachable>
	Attachable* FindAttachable();


	void AddBehaviour(AttachableBehaviour* behaviour);
private:
	void StartAttachables();
	virtual void StartCurrent();
	void StartChildren();

	void UpdateAttachables(sf::Time dt, CommandQueue& commands);
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands);
	void UpdateChildren(sf::Time dt, CommandQueue& commands);

	virtual void OnCollisionCurrent(SceneNode* other);
	void OnCollisionAttachables(SceneNode* other);


	//Note draw() is from sf::Drawable and hence the name
	//Do not be tempted to call this method Draw()
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	void DrawChildren(sf::RenderTarget& target, sf::RenderStates states) const;
	

	virtual bool IsMarkedForRemoval() const;


private:
	std::vector<Ptr> children_;
	ReceiverCategories default_category_;

	std::vector<AttachableBehaviour*> behaviours_;
	
	bool isDeleted_ = false;

	sf::Vector2f velocity_;

	World* world_;

	CollisionLayer collision_layer_;

	bool has_started_ = false;
protected:
	SceneNode* parent_;
public:
	std::string name_;
};
float Distance(const SceneNode& lhs, const SceneNode& rhs);
bool Collision(const SceneNode& lhs, const SceneNode& rhs);

template<typename Attachable> inline Attachable* SceneNode::FindAttachable() { 
	static_assert(std::is_base_of_v<AttachableBehaviour, Attachable>, "Attachable must derive from AttachableBehaviour"); 
	for (AttachableBehaviour* behaviour : behaviours_) { 
		if (auto* casted = dynamic_cast<Attachable*>(behaviour)) {
			return casted; 
		} 
	} 
	std::printf("Could not find attachable\n");
	return nullptr;
}