#include "attachable_behaviour.hpp"

AttachableBehaviour::AttachableBehaviour() {
	// this is used
}

AttachableBehaviour::~AttachableBehaviour() {
	node_ = nullptr;
}

void AttachableBehaviour::SetupBehaviour(SceneNode* node) {
	node_ = node;
}

void AttachableBehaviour::Start() {
}

void AttachableBehaviour::Update(sf::Time dt, CommandQueue& commands) {
}

void AttachableBehaviour::OnCollision(SceneNode* other) {
}

SceneNode* AttachableBehaviour::GetNode() {
	return node_;
}
