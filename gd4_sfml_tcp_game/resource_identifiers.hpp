#pragma once
#include "texture_id.hpp"
#include "tile_id.hpp"
#include "font.hpp"
#include "shader_types.hpp"
#include "sound_effect.hpp"

namespace sf
{
	class Texture;
	class Font;
	class Shader;
	class SoundBuffer;
}

template<typename Identifier, typename Resource>
class ResourceHolder;

typedef ResourceHolder<TextureID, sf::Texture> TextureHolder;
typedef ResourceHolder<TileID, sf::Texture> TileSheetHolder;
typedef ResourceHolder <Font, sf::Font > FontHolder;
typedef ResourceHolder<ShaderTypes, sf::Shader> ShaderHolder;
typedef ResourceHolder<SoundEffect, sf::SoundBuffer> SoundBufferHolder;