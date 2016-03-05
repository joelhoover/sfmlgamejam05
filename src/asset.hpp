#ifndef _ASSET_HPP_
#define _ASSET_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <string>
#include <unordered_map>
#include <memory>

class Asset
{
	std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
	std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> soundbuffers;
};

#endif // _ASSET_HPP_
