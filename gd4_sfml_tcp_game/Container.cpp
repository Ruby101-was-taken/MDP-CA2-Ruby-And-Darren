#include "container.hpp"

gui::Container::Container(int column_height)
    :selected_child_(-1),
    column_height_(column_height)
{
}

void gui::Container::Pack(Component::Ptr component)
{
    children_.emplace_back(component);
    if (!HasSelection() && component->IsSelectable())
    {
        Select(children_.size() - 1);
    }
}

bool gui::Container::IsSelectable() const
{
    return false;
}

void gui::Container::HandleEvent(const sf::Event& event)
{
    if (HasSelection() && children_[selected_child_]->IsActive())
    {
        children_[selected_child_]->HandleEvent(event);
    }
    const auto* key_pressed = event.getIf<sf::Event::KeyPressed>();
    if (key_pressed)
    {
        sf::Keyboard::Scancode key = key_pressed->scancode;
        if (key == sf::Keyboard::Scancode::W || key == sf::Keyboard::Scancode::Up)
        {
            SelectPrevious(1);
        }
        else if (key == sf::Keyboard::Scancode::A || key == sf::Keyboard::Scancode::Left) {
            SelectPrevious(column_height_);
        }
        else if (key == sf::Keyboard::Scancode::S || key == sf::Keyboard::Scancode::Down)
        {
            SelectNext(1);
        }
        else if (key == sf::Keyboard::Scancode::D || key == sf::Keyboard::Scancode::Right)
        {
            SelectNext(column_height_);
        }
        else if (key == sf::Keyboard::Scancode::Enter || key == sf::Keyboard::Scancode::Space)
        {
            if (HasSelection())
            {
                children_[selected_child_]->Activate();
            }
        }
    }
}

void gui::Container::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    for (const Component::Ptr& child : children_)
    {
        target.draw(*child, states);
    }
}

bool gui::Container::HasSelection() const
{
    return selected_child_ >= 0;
}

void gui::Container::Select(std::size_t index)
{
    if (index < children_.size() && children_[index]->IsSelectable())
    {
        if (HasSelection())
        {
            children_[selected_child_]->Deselect();
        }
        children_[index]->Select();
        selected_child_ = index;
    }
}

void gui::Container::SelectNext(int move)
{
    if (!HasSelection())
    {
        return;
    }
    //Search for the next selectable component
    int next = selected_child_;
    do
    {
        next = (next + move) % children_.size();
    } while (!children_[next]->IsSelectable());
    Select(next);
}

void gui::Container::SelectPrevious(int move)
{
    if (!HasSelection())
    {
        return;
    }
    int prev = selected_child_;
    do
    {
        prev = (prev + children_.size() - move) % children_.size();
    } while (!children_[prev]->IsSelectable());
    Select(prev);
}
