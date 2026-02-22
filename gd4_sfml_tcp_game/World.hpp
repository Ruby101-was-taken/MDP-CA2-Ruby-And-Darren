#pragma once
#include <SFML/Graphics.hpp>
#include "resource_identifiers.hpp"
#include "resource_holder.hpp"
#include "scene_node.hpp"
#include "scene_layers.hpp"
#include "texture_id.hpp"
#include "sprite_node.hpp"
#include "command_queue.hpp"
#include "bloom_effect.hpp"
#include "sound_player.hpp"
#include "state.hpp"

#include <array>

class World 
{
public:
	typedef std::unique_ptr<SceneNode> Ptr;

	explicit World(sf::RenderTarget& target, FontHolder& font, SoundPlayer& sounds, State::Context* context);
	void Update(sf::Time dt);

	virtual void RenderLogic();

	CommandQueue& GetCommandQueue();

	void AddNode(Ptr scene_node);

	State::Context* GetContext();

	void SetWinningPlayer(ReceiverCategories payer_category);
	ReceiverCategories GetWinningPlayer() const;
	bool LevelHasEnded() const;
	void SetState(State* state);

	State* GetState();

protected:
	void SetCameraPosition(sf::Vector2f position);
	void SetCameraSize(sf::Vector2f position);

	void DrawWorld();
	void ClearScreen();
	void Draw(const sf::Drawable& object);
	void ApplyPostEffects();

	void LoadTextures();
	
	sf::FloatRect GetViewBounds() const;
	sf::FloatRect GetBattleFieldBounds() const;

	void DestroyEntitiesOutsideView();

	void HandleCollisions();
	void UpdateSounds();

	void StartBuildScene();
	virtual void BuildScene();

protected:
	SceneNode root_node_;
	TextureHolder textures_;
	FontHolder& fonts_;
	SoundPlayer& sounds_;
	sf::RenderTarget& target_;
	sf::RenderTexture scene_texture_;
	sf::View camera_;
	std::array<SceneNode*, static_cast<int>(SceneLayers::kLayerCount)> scene_layers_;
	sf::FloatRect world_bounds_;
	sf::Vector2f spawn_position_;
	float scrollspeed_;

	CommandQueue command_queue_;


	BloomEffect bloom_effect_;

	State::Context* context_;
	State* state_;

	bool has_level_;
	std::string level_path_;
	ReceiverCategories winning_player;
	bool level_is_over = false;
};

