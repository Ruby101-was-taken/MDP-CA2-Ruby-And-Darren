#include "texture_holder.hpp"
#include <cassert>

void TextureHolder::Load(TextureID id, const std::string& filename)
{
    std::unique_ptr<sf::Texture> texture(new sf::Texture());
    if (!texture->loadFromFile(filename))
    {
        throw std::runtime_error("TextureHolder::Load failed to load " + filename);
    }
    auto inserted = texture_map_.insert(std::make_pair(id, std::move(texture)));
    assert(inserted.second);
}

sf::Texture& TextureHolder::Get(TextureID id)
{
    auto found = texture_map_.find(id);
    assert(found != texture_map_.end());
    return *found->second;
}

const sf::Texture& TextureHolder::Get(TextureID id) const
{
    auto found = texture_map_.find(id);
    assert(found != texture_map_.end());
    return *found->second;
}
