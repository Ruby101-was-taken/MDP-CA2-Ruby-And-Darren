#pragma once
#include "resource_identifiers.hpp"
#include "state_id.hpp"
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include "music_player.hpp"
#include "sound_player.hpp"
#include "player_info.hpp"
//#include "level.hpp"

class Level;

namespace sf
{
	class RenderWindow;
}

class StateStack;

class State
{
public:
	typedef std::unique_ptr<State> Ptr;

	struct Context
	{
		Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, MusicPlayer& music, SoundPlayer& sounds);
		sf::RenderWindow* window;
		TextureHolder* textures;
		FontHolder* fonts;
		MusicPlayer* music;
		SoundPlayer* sounds;
	};

public:
	State(StateStack& stack, Context context);
	virtual ~State();
	virtual void Draw() = 0;
	virtual bool Update(sf::Time dt) = 0;
	virtual bool HandleEvent(const sf::Event& event) = 0;

#pragma region UI
	virtual void ShowNewName(PlayerInfo info, bool is_host = true);
	virtual void RemoveName(std::string name);
	virtual void SetLobbyTitle(const std::string& title);
#pragma endregion

	virtual std::vector<PlayerInfo> GetNames();
	virtual void ExitLobbyState();
	void SetIsHost(bool is_host);

	Context GetContext() const;
protected:
	void RequestStackPush(StateID state_id);
	void RequestStackPop();
	void RequestStackClear();


protected:
	bool is_host_ = false;

private:
	StateStack* stack_;
	Context context_;
;
};

