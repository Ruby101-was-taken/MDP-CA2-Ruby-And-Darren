// Ruby White - D00255322 - Whole Class
#pragma once
#include "attachable_behaviour.hpp"
#include "box_collider_behaviour.hpp"

class PhysicBody : public AttachableBehaviour {
public:
	PhysicBody(BoxColliderBehaviour* collider, float acceleration_speed, float ground_deceleration_speed, float air_deceleration_speed, float max_speed, float coyote_time);
	PhysicBody(BoxColliderBehaviour* collider);
	void Update(sf::Time dt, CommandQueue& commands) override;

	sf::Vector2f& GetVelocity();

protected:
	// allow derived classes to query ground contact
	bool IsOnGround();

	// allow derived classes to provide custom physics behaviour
	virtual sf::Vector2f CustomPhysicsUpdate(sf::Time dt, CommandQueue& commands) = 0;

	// movement helpers available to derived classes if needed
	void PerformGravity();
	float MoveInDirection(float speed, sf::Vector2f direction);
	void PerformDeceleration();

protected:

	float acceleration_speed_;
	float deceleration_speed_;
	float ground_deceleration_speed_;
	float air_deceleration_speed_;
	float max_speed_;

	float default_coyote_time_;
	float coyote_time_;


	BoxColliderBehaviour* collider_;

	sf::Vector2f velocity_;
};

