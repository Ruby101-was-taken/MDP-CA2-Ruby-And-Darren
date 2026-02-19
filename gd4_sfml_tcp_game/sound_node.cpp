#include "sound_node.hpp"

#include "sound_player.hpp"
#include "receiver_categories.hpp"


SoundNode::SoundNode(SoundPlayer& player)
	: sounds_(player)
{
}

void SoundNode::PlaySound(SoundEffect sound, sf::Vector2f position)
{
	sounds_.Play(sound);
}

unsigned int SoundNode::GetCategory() const
{
	return static_cast<int>(ReceiverCategories::kSoundEffect);
}
