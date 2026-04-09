#pragma once
#include "Component.hpp"

namespace gui
{
	class Container : public Component
	{
	public:
		typedef std::shared_ptr<Container> Ptr;

	public:
		Container(int column_height=1);
		void Pack(Component::Ptr component);
		virtual bool IsSelectable() const override;
		virtual void HandleEvent(const sf::Event& event) override;

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		bool HasSelection() const;
		void Select(std::size_t index);
		void SelectNext(int move = 1);
		void SelectPrevious(int move = 1);

	private:
		std::vector<Component::Ptr> children_;
		int selected_child_;
		int column_height_;
	};
}

