#include "attachable_behaviour.hpp"

AttachableBehaviour::AttachableBehaviour() {
	// I don't think this runs idk why it is here oopsies
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
