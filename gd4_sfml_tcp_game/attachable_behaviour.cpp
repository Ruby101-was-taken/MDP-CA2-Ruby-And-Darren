#include "attachable_behaviour.hpp"
#include "sound_node.hpp"

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

void AttachableBehaviour::PlayLocalSound(CommandQueue& commands, SoundEffect effect)
{
	sf::Vector2f world_position = node_->GetWorldPosition();

	Command command;
	command.category = static_cast<int>(ReceiverCategories::kSoundEffect);
	command.action = DerivedAction<SoundNode>(
		[effect, world_position](SoundNode& node, sf::Time)
		{
			node.PlaySound(effect, world_position);
		});

	commands.Push(command);
}

SceneNode* AttachableBehaviour::GetNode() {
	return node_;
}
