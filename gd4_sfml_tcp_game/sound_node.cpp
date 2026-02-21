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

ReceiverCategories SoundNode::GetCategoryEnum() const
{
	return ReceiverCategories::kSoundEffect;
}
