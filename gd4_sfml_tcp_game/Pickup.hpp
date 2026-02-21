#pragma once
#include "entity.hpp"
#include "pickup_type.hpp"
#include "resource_identifiers.hpp"

class Aircraft;

class Pickup : public Entity
{
public:
	Pickup(PickupType type, const TextureHolder& textures);
	virtual ReceiverCategories GetCategoryEnum() const override;
	virtual sf::FloatRect GetBoundingRect() const;
	void Apply(Aircraft& player) const;
	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	PickupType type_;
	sf::Sprite sprite_;
};

