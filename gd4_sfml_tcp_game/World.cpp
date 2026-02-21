#include "world.hpp"
#include "pickup.hpp"
#include "Utility.hpp"
#include "level.hpp"
#include <iostream>

World::World(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context)
	:target_(output_target)
	, camera_(output_target.getDefaultView())
	, textures_()
	, fonts_(font)
	, sounds_(sounds)
	, root_node_(0, 0, ReceiverCategories::kNone)
	, scene_layers_()
	, world_bounds_({ 0.f,0.f }, { camera_.getSize().x, 3000.f })
	, spawn_position_(0, 0)
	, scrollspeed_(-50.f)
	, scene_texture_({ target_.getSize().x, target_.getSize().y }),
	context_(context),
	level_path_("data.csv")
{
	camera_.setSize({ 640, 360});
	LoadTextures();
	StartBuildScene();
	camera_.setCenter({320, 180});
}


void World::Update(sf::Time dt)
{
	//Forward commands to the scenegraph
	while (!command_queue_.IsEmpty())
	{
		root_node_.OnCommand(command_queue_.Pop(), dt);
	}

	HandleCollisions();

	root_node_.Update(dt, command_queue_);	
}

void World::Draw()
{
	if (PostEffect::IsSupported())
	{
		scene_texture_.clear();
		scene_texture_.setView(camera_);
		sf::Sprite level(Level::level_texture_.getTexture());
		scene_texture_.draw(level);
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

State::Context* World::GetContext() {
	return context_;
}


void World::LoadTextures()
{
	textures_.Load(TextureID::kPlayerOne, "Media/Textures/Player/One/Idle.png");
	textures_.Load(TextureID::kPlayerOneSheet, "Media/Textures/Player/One/Sheet.png");
	textures_.Load(TextureID::kPlayerTwo, "Media/Textures/Player/Two/Idle.png");
	textures_.Load(TextureID::kPlayerTwoSheet, "Media/Textures/Player/Two/Sheet.png");
	textures_.Load(TextureID::kItemStar, "Media/Textures/Item/Star.png");
	textures_.Load(TextureID::kLevelTile, "Media/Textures/Level/Tile.png");
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

	Level::LoadLevel(level_path_, textures_.Get(TextureID::kLevelTile));

	root_node_.SetWorld(this);
	BuildScene();
	root_node_.Start();
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

void World::DestroyEntitiesOutsideView() {
	//Command command;
	//command.category = static_cast<int>(ReceiverCategories::kPlayer2) | static_cast<int>(ReceiverCategories::kProjectile);
	//command.action = DerivedAction<Entity>([this](Entity& e, sf::Time dt)
	//	{
	//		//Does the object intersect with the battlefield
	//		if (!GetBattleFieldBounds().findIntersection(e.GetBoundingRect()).has_value())
	//		{
	//			e.Destroy();
	//		}
	//	});
	//command_queue_.Push(command);
}



void World::HandleCollisions()
{
	std::vector<BaseColliderBehaviour*> colliders;
	root_node_.CollectColliders(colliders);

	for (auto* collider : colliders) {
		collider->BeginCollisionFrame(); // clears currentCollisions_
	}

	const size_t count = colliders.size();
	for (size_t i = 0; i < count; ++i) {
		// start the second iterration later so we don't test already tested collisions
		for (size_t j = i + 1; j < count; ++j) {
			
			auto* a = colliders[i];
			auto* b = colliders[j];
			
			bool intersection = a->IsCollding(b);
			if (intersection) {
				a->RegisterCollision(b->GetNode());
				b->RegisterCollision(a->GetNode());
			}
		}
	}

	for (auto* collider : colliders) {
		collider->EndCollisionFrame();
	}
}

void World::UpdateSounds()
{
	// Set listener's position to player position
	
	sounds_.SetListenerPosition({680, 180}); //basic position for right now we need to change this when we do a dynamic camera but for rn we are all cool with a static camera and a static sound listener ok thank you for listening
	// TODO: Set listener to center of level
	// Remove unused sounds
	sounds_.RemoveStoppedSounds();
}
