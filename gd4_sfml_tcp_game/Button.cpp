#include "button.hpp"
#include "resource_holder.hpp"
#include "utility.hpp"
#include "button_type.hpp"

gui::Button::Button(State::Context context)
    : sprite_(context.textures->Get(TextureID::kButtons))
    , text_(context.fonts->Get(Font::kMain), "", 16)
    , is_toggle_(false)
    , sounds_(*context.sounds)
{
    ChangeTexture(ButtonType::kNormal);
    sf::FloatRect bounds = sprite_.getLocalBounds();
    text_.setPosition({ bounds.size.x / 2, bounds.size.y / 2 });
}

void gui::Button::SetCallback(Callback callback)
{
    callback_ = std::move(callback);
}

void gui::Button::SetText(const std::string& text)
{
    text_.setString(text);
    Utility::CentreOrigin(text_);
}

void gui::Button::SetToggle(bool flag)
{
    is_toggle_ = flag;
}

bool gui::Button::IsSelectable() const
{
    return true;
}

void gui::Button::Select()
{
    Component::Select();
    ChangeTexture(ButtonType::kSelected);
    sounds_.Play(SoundEffect::kButtonSelected);
}

void gui::Button::Deselect()
{
    Component::Deselect();
    ChangeTexture(ButtonType::kNormal);
}

void gui::Button::Activate()
{
    Component::Activate();

    //If toggle button then show the button is activated
    if (is_toggle_)
    {
        ChangeTexture(ButtonType::kPressed);
    }
    if (callback_)
    {
        callback_();
    }
    if (!is_toggle_)
    {
        Deactivate();
    }
    sounds_.Play(SoundEffect::kButtonClicked);
}

void gui::Button::Deactivate()
{
    Component::Deactivate();
    if (is_toggle_)
    {
        if (IsSelected())
        {
            ChangeTexture(ButtonType::kSelected);
        }
        else
        {
            ChangeTexture(ButtonType::kNormal);
        }
    }
}

void gui::Button::HandleEvent(const sf::Event& event)
{
}

void gui::Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(sprite_, states);
    target.draw(text_, states);
}

// Darren
void gui::Button::ChangeTexture(ButtonType buttonType)
{
    sf::IntRect textureRect; // Account for the different dimensions of each button type
    if (buttonType == ButtonType::kNormal)
        textureRect = sf::IntRect({ 0, 0 }, { 160, 50 });
    else if (buttonType == ButtonType::kSelected)
        textureRect = sf::IntRect({ 0, 50 }, { 160, 70 });
    else
        textureRect = sf::IntRect({ 0, 100 }, { 160, 50 });

    sprite_.setTextureRect(textureRect);

    // Re-center text
    sf::FloatRect bounds = sprite_.getLocalBounds();
    text_.setPosition({ bounds.size.x / 2, bounds.size.y / 2 });
}