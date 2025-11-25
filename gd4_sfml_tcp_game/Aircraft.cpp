#include "aircraft.hpp"
#include "texture_id.hpp"
#include "resource_holder.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include "data_tables.hpp"
#include "projectile.hpp"
#include "pickup_type.hpp"
#include "pickup.hpp"
#include "sound_node.hpp"
#include "health_behaviour.hpp"

namespace
{
	const std::vector<AircraftData> Table = InitializeAircraftData();
}

TextureID ToTextureID(AircraftType type)
{
	switch (type)
	{
	case AircraftType::kEagle:
		return TextureID::kEagle;
		break;
	case AircraftType::kRaptor:
		return TextureID::kRaptor;
		break;
	case AircraftType::kAvenger:
		return TextureID::kAvenger;
		break;
	}
	return TextureID::kEagle;
}

Aircraft::Aircraft(AircraftType type, const TextureHolder& textures, const FontHolder& fonts)  
	: Entity(Table[static_cast<int>(type)].hitpoints)
	, type_(type)
	, sprite_(textures.Get(Table[static_cast<int>(type)].texture), Table[static_cast<int>(type)].texture_rect)
	, explosion_(textures.Get(TextureID::kExplosion))
	, health_display_(nullptr)
	, missile_display_(nullptr)
	, distance_travelled_(0.f)
	, directions_index_(0)
	, fire_rate_(1)
	, spread_level_(1)
	, is_firing_(false)
	, is_launching_missile_(false)
	, fire_countdown_(sf::Time::Zero)
	, missile_ammo_(2)
	, is_marked_for_removal_(false)
	, show_explosion_(true)
	, spawned_pickup_(false)
	, played_explosion_sound_(false)

{

	explosion_.SetFrameSize(sf::Vector2i(256, 256));
	explosion_.SetNumFrames(16);
	explosion_.SetDuration(sf::seconds(1));
	Utility::CentreOrigin(sprite_);
	Utility::CentreOrigin(explosion_);

	fire_command_.category = static_cast<int>(ReceiverCategories::kScene);
	fire_command_.action = [this, &textures](SceneNode& node, sf::Time dt)
		{
			CreateBullet(node, textures);
		};

	missile_command_.category = static_cast<int>(ReceiverCategories::kScene);
	missile_command_.action = [this, &textures](SceneNode& node, sf::Time dt)
		{
			CreateProjectile(node, ProjectileType::kMissile, 0.f, 0.5f, textures);
		};

	drop_pickup_command_.category = static_cast<int>(ReceiverCategories::kScene);
	drop_pickup_command_.action = [this, &textures](SceneNode& node, sf::Time dt)
		{
			CreatePickup(node, textures);
		};

	std::string* health = new std::string("");
	std::unique_ptr<TextNode> health_display(new TextNode(fonts, *health));
	health_display_ = health_display.get();
	AttachChild(std::move(health_display));

	if (Aircraft::GetCategory() == static_cast<int>(ReceiverCategories::kPlayerAircraft))
	{
		std::string* missile_ammo = new std::string("");
		std::unique_ptr<TextNode> missile_display(new TextNode(fonts, *missile_ammo));
		missile_display_ = missile_display.get();
		AttachChild(std::move(missile_display));
		//AddBehaviour(new HealthBehaviour());
	}

	UpdateTexts();
}

unsigned int Aircraft::GetCategory() const
{
	if (IsAllied())
	{
		return static_cast<unsigned int>(ReceiverCategories::kPlayerAircraft);
	}
	return static_cast<unsigned int>(ReceiverCategories::kEnemyAircraft);

}

void Aircraft::IncreaseFireRate()
{
	if (fire_rate_ < 5)
	{
		++fire_rate_;
	}
}

void Aircraft::IncreaseFireSpread()
{
	if (spread_level_ < 3)
	{
		++spread_level_;
	}
}

void Aircraft::CollectMissile(unsigned int count)
{
	missile_ammo_ += count;
}

void Aircraft::UpdateTexts()
{
	health_display_->SetString(std::to_string(GetHitPoints()) + "HP");
	health_display_->setPosition({ 0.f, 50.f });
	health_display_->setRotation(-getRotation());

	if (missile_display_)
	{
		missile_display_->setPosition({ 0.f, 70.f });
		if (missile_ammo_ == 0)
		{
			missile_display_->SetString("");
		}
		else
		{
			missile_display_->SetString("M: " + std::to_string(missile_ammo_));
		}
	}
}

void Aircraft::UpdateMovementPattern(sf::Time dt)
{
	//Enemy AI
	const std::vector<Direction>& directions = Table[static_cast<int>(type_)].directions;
	if (!directions.empty())
	{
		//Move along the current direction, then change direction
		if (distance_travelled_ > directions[directions_index_].distance)
		{
			directions_index_ = (directions_index_ + 1) % directions.size();
			distance_travelled_ = 0.f;
		}

		//Compute velocity
		//Add 90 to move down the screen, 0 is right

		double radians = Utility::ToRadians(directions[directions_index_].angle + 90.f);
		float vx = GetMaxSpeed() * std::cos(radians);
		float vy = GetMaxSpeed() * std::sin(radians);

		SetVelocity(vx, vy);
		distance_travelled_ += GetMaxSpeed() * dt.asSeconds();
	}
}

float Aircraft::GetMaxSpeed() const
{
	return Table[static_cast<int>(type_)].speed;
}

void Aircraft::Fire()
{
	if (Table[static_cast<int>(type_)].fire_interval != sf::Time::Zero)
	{
		is_firing_ = true;
	}
}


void Aircraft::LaunchMissile()
{
	if (missile_ammo_ > 0)
	{
		is_launching_missile_ = true;
		--missile_ammo_;
	}
}

void Aircraft::CreateBullet(SceneNode& node, const TextureHolder& textures) const
{
	ProjectileType type = IsAllied() ? ProjectileType::kAlliedBullet : ProjectileType::kEnemyBullet;
	switch (spread_level_)
	{
	case 1:
		CreateProjectile(node, type, 0.0f, 0.5f, textures);
		break;
	case 2:
		CreateProjectile(node, type, -0.5f, 0.5f, textures);
		CreateProjectile(node, type, 0.5f, 0.5f, textures);
		break;
	case 3:
		CreateProjectile(node, type, 0.0f, 0.5f, textures);
		CreateProjectile(node, type, -0.5f, 0.5f, textures);
		CreateProjectile(node, type, 0.5f, 0.5f, textures);
		break;
	}
	
}

void Aircraft::CreateProjectile(SceneNode& node, ProjectileType type, float x_offset, float y_offset, const TextureHolder& textures) const
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));
	sf::Vector2f offset(x_offset * sprite_.getGlobalBounds().size.x, y_offset * sprite_.getGlobalBounds().size.y);
	sf::Vector2f velocity(0, projectile->GetMaxSpeed());

	float sign = IsAllied() ? -1.f : 1.f;
	projectile->setPosition(GetWorldPosition() + offset * sign);
	projectile->SetVelocity(velocity* sign);
	node.AttachChild(std::move(projectile));
}

sf::FloatRect Aircraft::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(sprite_.getGlobalBounds());
}

bool Aircraft::IsMarkedForRemoval() const
{
	return IsDestroyed() && (explosion_.IsFinished() || !show_explosion_);
}

void Aircraft::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (IsDestroyed() && show_explosion_)
	{
		target.draw(explosion_, states);
	}
	else
	{
		target.draw(sprite_, states);
	}
}

void Aircraft::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (IsDestroyed())
	{
		CheckPickupDrop(commands);
		explosion_.Update(dt);
		// Play explosion sound only once
		if (!played_explosion_sound_)
		{
			SoundEffect soundEffect = (Utility::RandomInt(2) == 0) ? SoundEffect::kExplosion1 : SoundEffect::kExplosion2;
			PlayLocalSound(commands, soundEffect);

			played_explosion_sound_ = true;
		}
		return;
	}

	Entity::UpdateCurrent(dt, commands);
	UpdateTexts();
	UpdateMovementPattern(dt);

	UpdateRollAnimation();

	//Check if bullets or misiles are fired
	CheckProjectileLaunch(dt, commands);
}

void Aircraft::CheckProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	if (!IsAllied())
	{
		Fire();
	}

	if (is_firing_ && fire_countdown_ <= sf::Time::Zero)
	{
		PlayLocalSound(commands, IsAllied() ? SoundEffect::kEnemyGunfire : SoundEffect::kAlliedGunfire);
		commands.Push(fire_command_);
		fire_countdown_ += Table[static_cast<int>(type_)].fire_interval / (fire_rate_ + 1.f);
		is_firing_ = false;
	}
	else if (fire_countdown_ > sf::Time::Zero)
	{
		//Wait, can't fire
		fire_countdown_ -= dt;
		is_firing_ = false;
	}

	//Missile launch
	if (is_launching_missile_)
	{
		PlayLocalSound(commands, SoundEffect::kLaunchMissile);
		commands.Push(missile_command_);
		is_launching_missile_ = false;
	}
}

bool Aircraft::IsAllied() const
{
	return type_ == AircraftType::kEagle;
}

void Aircraft::CreatePickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<PickupType>(Utility::RandomInt(static_cast<int>(PickupType::kPickupCount)));
	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(GetWorldPosition());
	pickup->SetVelocity(0.f, 0.f);
	node.AttachChild(std::move(pickup));
}

void Aircraft::CheckPickupDrop(CommandQueue& commands)
{
	//TODO Get rid of the magic number 3 here 
	if (!IsAllied() && Utility::RandomInt(3) == 0 && !spawned_pickup_)
	{
		commands.Push(drop_pickup_command_);
	}
	spawned_pickup_ = true;
}

void Aircraft::UpdateRollAnimation() {
	if (Table[static_cast<int>(type_)].has_roll_animation)
	{
		sf::IntRect textureRect = Table[static_cast<int>(type_)].texture_rect;

		//Roll left: Texture rect is offset once
		if (GetVelocity().x < 0.f)
		{
			textureRect.position.x += textureRect.size.x;
		}
		else if (GetVelocity().x > 0.f)
		{
			textureRect.position.x += 2 * textureRect.size.x;
		}
		sprite_.setTextureRect(textureRect);

	}
}

void Aircraft::PlayLocalSound(CommandQueue& commands, SoundEffect effect)
{
	sf::Vector2f world_position = GetWorldPosition();

	Command command;
	command.category = static_cast<int>(ReceiverCategories::kSoundEffect);
	command.action = DerivedAction<SoundNode>(
		[effect, world_position](SoundNode& node, sf::Time)
		{
			node.PlaySound(effect, world_position);
		});

	commands.Push(command);
}
