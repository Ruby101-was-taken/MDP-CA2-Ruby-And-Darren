// Darren - D00255479
#pragma once
#include "Component.hpp"
#include <functional>
#include "resource_identifiers.hpp"
#include "sound_player.hpp"
#include "state.hpp"
#include "button_type.hpp"

namespace gui
{
	class Button : public Component
	{
	public:
		typedef std::shared_ptr<Button> Ptr;
		typedef std::function<void()> Callback;

	public:
		Button(State::Context context);
		void SetCallback(Callback callback);
		void SetText(const std::string& text);
		void SetToggle(bool flag);
		void SetSmall(bool flag); // Darren - D00255479

		virtual bool IsSelectable() const override;
		virtual void Select() override;
		virtual void Deselect() override;
		virtual void Activate() override;
		virtual void Deactivate() override;
		virtual void HandleEvent(const sf::Event& event) override;

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
		void ChangeTexture(ButtonType buttonType);

	private:
		Callback callback_;
		sf::Sprite sprite_;
		sf::Sprite small_sprite_; // Darren - D00255479
		sf::Text text_;
		bool is_toggle_;
		bool is_small_; // Darren - D00255479
		SoundPlayer& sounds_;
	};
}

