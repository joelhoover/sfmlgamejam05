#ifndef _ASSET_HPP_
#define _ASSET_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <cassert>

#include <string>
#include <unordered_map>
#include <memory>

#include "common.hpp"
#include "resource_path.hpp"

class Asset
{
public:


	void load_shader(const std::string& tag, const std::string& file_vert, const std::string& file_frag)
	{
		auto emplace_result = shaders.emplace(tag, std::make_unique<sf::Shader>());
		assert(emplace_result.second);
		assert( emplace_result.first->second.get()->loadFromFile(resource_path() + file_vert, resource_path() + file_frag) );
	}

	void load_font(const std::string& tag, const std::string& filepath)
	{
		auto emplace_result = fonts.emplace(tag, std::make_unique<sf::Font>());
		assert(emplace_result.second);
		assert( emplace_result.first->second.get()->loadFromFile(resource_path() + filepath) );
	}

	void load_texture(const std::string& tag, const std::string& filepath)
	{
		auto emplace_result = textures.emplace(tag, std::make_unique<sf::Texture>());
		sf::Texture* tex = emplace_result.first->second.get();
		assert( tex->loadFromFile(resource_path() + filepath) );
	}

	void load_soundbuffer(const std::string& tag, const std::string& filepath)
	{
		auto emplace_result = soundbuffers.emplace(tag, std::make_unique<sf::SoundBuffer>());
		sf::SoundBuffer* sb = emplace_result.first->second.get();
		assert( sb->loadFromFile(resource_path() + filepath) );
	}

	std::unordered_map<std::string, std::unique_ptr<sf::Shader>> shaders;
	std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;
	std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
	std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> soundbuffers;
};

#endif // _ASSET_HPP_
