#include "world.hpp"
#include "pickup.hpp"
#include "sound_node.hpp"

World::World(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds)
	:target_(output_target)
	,camera_(output_target.getDefaultView())
	,textures_()
	,fonts_(font)
	,sounds_(sounds)
	,root_node_(0, 0,ReceiverCategories::kNone)
	,scene_layers_()
	,world_bounds_({ 0.f,0.f }, { camera_.getSize().x, 3000.f })
	,spawn_position_(0,0)
	,scrollspeed_(-50.f)
	,scene_texture_({ target_.getSize().x, target_.getSize().y })
{
	LoadTextures();
	StartBuildScene();
	camera_.setCenter(spawn_position_);
}


void World::Update(sf::Time dt)
{
	//Forward commands to the scenegraph
	while (!command_queue_.IsEmpty())
	{
		root_node_.OnCommand(command_queue_.Pop(), dt);
	}

	root_node_.Update(dt, command_queue_);
}

void World::Draw()
{
	if (PostEffect::IsSupported())
	{
		scene_texture_.clear();
		scene_texture_.setView(camera_);
		scene_texture_.draw(root_node_);
		scene_texture_.display();
		bloom_effect_.Apply(scene_texture_, target_);
	}
	else
	{
		target_.setView(camera_);
		target_.draw(root_node_);
	}
}

CommandQueue& World::GetCommandQueue()
{
	return command_queue_;
}

typedef std::unique_ptr<SceneNode> Ptr;
void World::AddNode(Ptr scene_node) {
	root_node_.AttachChild(std::move(scene_node));
}


void World::LoadTextures()
{
	textures_.Load(TextureID::kEagle, "Media/Textures/Eagle.png");
	//textures_.Load(TextureID::kRaptor, "Media/Textures/Raptor.png");
	//textures_.Load(TextureID::kAvenger, "Media/Textures/Avenger.png");
	//textures_.Load(TextureID::kLandscape, "Media/Textures/Desert.png");
	//textures_.Load(TextureID::kBullet, "Media/Textures/Bullet.png");
	//textures_.Load(TextureID::kMissile, "Media/Textures/Missile.png");

	//textures_.Load(TextureID::kHealthRefill, "Media/Textures/HealthRefill.png");
	//textures_.Load(TextureID::kMissileRefill, "Media/Textures/MissileRefill.png");
	//textures_.Load(TextureID::kFireSpread, "Media/Textures/FireSpread.png");
	//textures_.Load(TextureID::kFireRate, "Media/Textures/FireRate.png");
	//textures_.Load(TextureID::kFinishLine, "Media/Textures/FinishLine.png");

	//textures_.Load(TextureID::kEntities, "Media/Textures/Entities.png");
	//textures_.Load(TextureID::kJungle, "Media/Textures/Jungle.png");
	//textures_.Load(TextureID::kExplosion, "Media/Textures/Explosion.png");
	//textures_.Load(TextureID::kParticle, "Media/Textures/Particle.png");


}

void World::StartBuildScene()
{
	//Initialize the different layers
	for (std::size_t i = 0; i < static_cast<int>(SceneLayers::kLayerCount); ++i)
	{
		ReceiverCategories category = (i == static_cast<int>(SceneLayers::kLowerAir)) ? ReceiverCategories::kScene : ReceiverCategories::kNone;
		SceneNode::Ptr layer(new SceneNode(0,0,category));
		scene_layers_[i] = layer.get();
		root_node_.AttachChild(std::move(layer));
	}

	root_node_ = SceneNode();
	BuildScene();
}
void World::BuildScene() {
}


sf::FloatRect World::GetViewBounds() const
{
	return sf::FloatRect(camera_.getCenter() - camera_.getSize()/2.f, camera_.getSize());
}

sf::FloatRect World::GetBattleFieldBounds() const
{
	//Return camera bounds + a small area at the top where enemies spawn
	sf::FloatRect bounds = GetViewBounds();
	bounds.position.y -= 100.f;
	bounds.size.y += 100.f;

	return bounds;

}

void World::DestroyEntitiesOutsideView()
{
	Command command;
	command.category = static_cast<int>(ReceiverCategories::kEnemyAircraft) | static_cast<int>(ReceiverCategories::kProjectile);
	command.action = DerivedAction<Entity>([this](Entity& e, sf::Time dt)
		{
			//Does the object intersect with the battlefield
			if (!GetBattleFieldBounds().findIntersection(e.GetBoundingRect()).has_value())
			{
				e.Destroy();
			}
		});
	command_queue_.Push(command);
}

void World::GuideMissiles()
{
}

bool MatchesCategories(SceneNode::Pair& colliders, ReceiverCategories type1, ReceiverCategories type2)
{
	unsigned int category1 = colliders.first->GetCategory();
	unsigned int category2 = colliders.second->GetCategory();

	if (static_cast<int>(type1) & category1 && static_cast<int>(type2) & category2)
	{
		return true;
	}
	else if (static_cast<int>(type1) & category2 && static_cast<int>(type2) & category1)
	{ 
		std::swap(colliders.first, colliders.second);
	}
	else
	{
		return false;
	}
}


void World::HandleCollisions()
{
	//std::set<SceneNode::Pair> collision_pairs;
	//scenegraph_.CheckSceneCollision(scenegraph_, collision_pairs);
	//for (SceneNode::Pair pair : collision_pairs)
	//{
	//	if (MatchesCategories(pair, ReceiverCategories::kPlayerAircraft, ReceiverCategories::kEnemyAircraft))
	//	{
	//		auto& player = static_cast<Aircraft&>(*pair.first);
	//		auto& enemy = static_cast<Aircraft&>(*pair.second);
	//		//Collision response
	//		player.Damage(enemy.GetHitPoints());
	//		enemy.Destroy();
	//	}

	//	else if (MatchesCategories(pair, ReceiverCategories::kPlayerAircraft, ReceiverCategories::kPickup))
	//	{
	//		auto& player = static_cast<Aircraft&>(*pair.first);
	//		auto& pickup = static_cast<Pickup&>(*pair.second);
	//		//Collision response
	//		pickup.Apply(player);
	//		pickup.Destroy();
	//		player.PlayLocalSound(command_queue_, SoundEffect::kCollectPickup);
	//	}
	//	else if (MatchesCategories(pair, ReceiverCategories::kPlayerAircraft, ReceiverCategories::kEnemyProjectile) || MatchesCategories(pair, ReceiverCategories::kEnemyAircraft, ReceiverCategories::kAlliedProjectile))
	//	{
	//		auto& aircraft = static_cast<Aircraft&>(*pair.first);
	//		auto& projectile = static_cast<Projectile&>(*pair.second);
	//		//Collision response
	//		aircraft.Damage(projectile.GetDamage());
	//		projectile.Destroy();
	//	}
	//}
}

void World::UpdateSounds()
{
	// Set listener's position to player position
	sounds_.SetListenerPosition({0,0}); //basic position for right now we need to change this when we do a dynamic camera but for rn we are all cool with a static camera and a static sound listener ok thank you for listening

	// Remove unused sounds
	sounds_.RemoveStoppedSounds();
}
