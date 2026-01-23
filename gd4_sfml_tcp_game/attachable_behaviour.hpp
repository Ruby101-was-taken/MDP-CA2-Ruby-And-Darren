#pragma once
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include "command_queue.hpp"

/*
scene nodes now contain ATTACHABLES, which let behaviours be shared across many nodes
It also allows for behaviours to be separated out from eachother
*/

class SceneNode; // forward decalre cuz circular import otherwise :(

class AttachableBehaviour {
protected:
	SceneNode* node_;
public:
	AttachableBehaviour();
	virtual ~AttachableBehaviour();

	void SetupBehaviour(SceneNode* node);
	virtual void Start();

	virtual void Update(sf::Time dt, CommandQueue& commands) =0;

	virtual void Draw(sf::RenderTarget& target, sf::RenderStates states) const {}
};