#ifndef _ASSET_HPP_
#define _ASSET_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <string>
#include <unordered_map>
#include <memory>

class Asset
{
public:

	void load_texture(const std::string& tag, const std::string& filepath)
	{
		auto emplace_result = textures.emplace(tag, std::make_unique<sf::Texture>());
		sf::Texture* tex = emplace_result.first->second.get();
		assert( tex->loadFromFile(filepath) );
	}

	void load_soundbuffer(const std::string& tag, const std::string& filepath)
	{
		auto emplace_result = textures.emplace(tag, std::make_unique<sf::SoundBuffer>());
		sf::SoundBuffer* sb = emplace_result.first->second.get();
		assert( sb->loadFromFile(filepath) );
	}

	std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
	std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> soundbuffers;
};

#endif // _ASSET_HPP_
