#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include <SFML/Graphics.hpp>

#include "common.hpp"
#include "asset.hpp"
#include "persist.hpp"
#include "dialogue.hpp"

#include <vector>
#include <memory>


class MegaSprite : public sf::Sprite
{

public:

	/*MegaSprite()
	{
		use_types.push_back(USE_SPEECH);
	}*/

	uint dialogue_id;
	std::vector<uint> use_types;
	//uint use_types;
};

class Scene : public sf::Drawable
{

public:

	Scene(Asset& iasset, Dialogue& idialogue, Persist& ipersist)
		:
		asset(iasset),
		dialogue(idialogue),
		persist(ipersist)
	{

		sprites.emplace_back( std::make_unique<MegaSprite>() );

		MegaSprite* sprite = sprites.back().get();
		sprite->use_types.push_back(USE_SPEECH);
		sprite->setTexture( *asset.textures["placeholder"] );
		sprite->setPosition(200,100);
		//sprite.setScale(8,4);
	}

	MegaSprite* frob_has_sprite(sf::Vector2f pos_in_world)
	{
		for (auto& sprite : sprites)
		{
			if ( sprite->getGlobalBounds().contains( pos_in_world ) )
			{
				return sprite.get();
			}
		}
		return nullptr;
	}

	uint frob_type(sf::Vector2f pos_in_world, uint current_type)
	{
		for (auto& sprite : sprites)
		{
			if ( sprite->getGlobalBounds().contains( pos_in_world ) )
			{
				for ( uint ut : sprite->use_types)
				{
					if (current_type == ut) return current_type;
				}

				return sprite->use_types.size() ? sprite->use_types.front() : current_type;
			}
		}

		return current_type;
	}

	bool frob(sf::Vector2f pos_in_world)
	{
		for (auto& sprite : sprites)
		{
			if ( sprite->getGlobalBounds().contains( pos_in_world ) )
			{
				// trigger dialogue associated with this sprite

				// put scene in laid back mode til dialogue ends
				//   faded / paused / blank

				// player and dialogue-subject to enter dialogue mode
				//   stop moving, face each other, change animation

				// dialogue.front()
				//this scene back()

				dialogue.advance( sprite->dialogue_id );

				return true;
			}
		}
		return false;
	}

	void update()
	{

	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		for (auto& sprite : sprites)
		{
			target.draw(*sprite, states);
		}
	}

	std::vector< std::unique_ptr<MegaSprite> > sprites;

private:

	Asset& asset;
	Dialogue& dialogue;
	Persist& persist;
};

#endif // _SCENE_HPP_
