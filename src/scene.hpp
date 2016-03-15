#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include <SFML/Graphics.hpp>

#include "sfml_vector_math.hpp"

#include "common.hpp"
#include "asset.hpp"
#include "persist.hpp"
#include "dialogue.hpp"

#include "tilemap.hpp"
#include "pathfind.hpp"

#include <vector>
#include <memory>


class MegaSprite : public sf::Sprite
{

public:

	virtual void update()
	{
		if ( grid_path.size() )
		{

			// follow next path point
		}
	}

	std::vector<uint> grid_path;

	sf::Vector2f pos_last;
	uint speech_id;
	bool use_speech = false;
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
		renderscene.create(ZOOM_W, ZOOM_H, false);

		view = renderscene.getDefaultView();
		view.setCenter( { int(ZOOM_W/2),int(ZOOM_H/2)} );


		shader = asset.shaders["fader"].get();

		edit_tile_texture = asset.textures["box"].get();
	}

	void load_from_file(const std::string& file_name)
	{
		json data = shrapx::load_json(file_name);

		tilemap.reset();

		tilemap = std::make_unique<TileMap>( data["grid_size"].get<uint>(), 8 );

		for (auto& element : data["collision"])
		{
			tilemap->grid.push_back( element.get<uint>() );
		}

		assert( tilemap->grid.size() == tilemap->map_area );

		scene_file = data["bg"].get<std::string>();

		sprite_bg.setTexture( *asset.textures[scene_file] );
	}

	void save_to_file(const std::string& file_name)
	{
		json data;
		data["grid_size"] = tilemap->map_width;
		data["collision"] = json::array();
		for (bool element : tilemap->grid)
		{
			data["collision"].push_back( element?1:0 );
		}

		data["bg"] = scene_file;

		shrapx::save_text(file_name, data.dump(1));
	}

	MegaSprite* frob_has_sprite(const sf::Vector2f& pos_in_world)
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

	/*uint frob_type(sf::Vector2f pos_in_world, uint current_type)
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
	}*/

	void frob(sf::Vector2f pos_in_world)
	{
		auto pos = mouse_to_scene(pos_in_world);
		for (auto& sprite : sprites)
		{
			if ( sprite->getGlobalBounds().contains( pos ) )
			{
				if (sprite->use_speech)
				{
					defocus();

					dialogue.activate( sprite->speech_id );

					// player and dialogue-subject to enter dialogue mode
					//   stop moving, face each other, change animation

					return;
				}
			}
		}
	}

	sf::Vector2f mouse_to_scene(const sf::Vector2f& pos)
	{
		return view.getCenter() + pos - sf::Vector2f(ZOOM_WH, ZOOM_HH);
	}

	void toggle_grid_location(const sf::Vector2f& pos)
	{
		uint apos = tilemap->scene_to_address( mouse_to_scene(pos) );

		tilemap->toggle( apos );
	}

	void set_grid_location(const sf::Vector2f& pos, bool new_val)
	{
		uint apos = tilemap->scene_to_address( mouse_to_scene(pos) );

		tilemap->set(apos, new_val);
	}

	void update()
	{
		// shader uniforms
		if (m_focus)
		{
			if (saturation < saturation_refocus) saturation += 0.02f;
			if (value      < value_refocus     ) value      += 0.0025f;
		}
		else
		{
			if (saturation > saturation_defocus) saturation -= 0.02f;
			if (value      > value_defocus     ) value      -= 0.0025f;
		}

		shader->setParameter("saturation", saturation);
		shader->setParameter("value", value);

		// follow a sprite
		if (follow_sprite != nullptr)
		{
			sf::Vector2f pos = follow_sprite->getPosition();
			sf::Vector2f diff = pos - view.getCenter();
			sf::Vector2f view_change;

			// warp if far away
			if ( fabs(diff.x) > ZOOM_W ||
					 fabs(diff.y) > ZOOM_H )
			{
				view.setCenter(pos);
			}
			else
			{
				if      ( diff.x < -ZOOM_WQ ) view_change.x -= 0.1f;
				else if ( diff.x > +ZOOM_WQ ) view_change.x += 0.1f;
				if      ( diff.y < -ZOOM_HQ ) view_change.y -= 0.1f;
				else if ( diff.y > +ZOOM_HQ ) view_change.y += 0.1f;

				view.move( view_change );
			}
		}

		// individual updates
		for (auto& sprite : sprites)
		{
			if ( sprite->grid_path.size() )
			{
				// follow next path point

				auto pos = sprite->getPosition() - tilemap->address_to_scene( sprite->grid_path.back() );
				pos.x += 4;
				pos.y += 4;

				sprite->move( sf::Normalize(pos) );
			}

			sprite->update();
		}

		// constrain all sprite movement to grid
		for (auto& sprite : sprites)
		{
			auto pos = sprite->getPosition();

			//uint apos = uint(pos.x/tile_size) + uint(pos.y/tile_size) * grid_size;
			if ( tilemap->grid[ tilemap->scene_to_address(pos) ] )
				sprite->setPosition(sprite->pos_last);
			else
				sprite->pos_last = pos;
		}
	}

	void focus()
	{
		m_focus = true;
	}

	void defocus()
	{
		m_focus = false;
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.shader = shader; //->setParameter("myvar", 5.f);

		target.draw(sprite_bg, states);

		for (auto& sprite : sprites)
		{
			target.draw(*sprite, states);
		}

		if (m_edit)
		{

			sf::Sprite edit_tile;
			edit_tile.setColor(sf::Color::Magenta);
			edit_tile.setTexture( *edit_tile_texture );

			for (uint it = 0; it < tilemap->grid.size(); ++it)
			{
				sf::Vector2f pos = tilemap->address_to_scene(it);

				if (
	(pos.x > view.getCenter().x-ZOOM_WH && pos.x < view.getCenter().x+ZOOM_WH+8) &&
	(pos.y > view.getCenter().y-ZOOM_HH && pos.y < view.getCenter().y+ZOOM_HH+8) && tilemap->grid[it])
				{
					//sf::Color col = tilemap->grid[it] ? sf::Color::Magenta : sf::Color::Cyan;
					//edit_tile.setColor(col);
					edit_tile.setPosition(pos);

					target.draw(edit_tile, states);
				}
			}
		}
	}

	std::vector<std::unique_ptr<MegaSprite>> sprites;

	std::unique_ptr<TileMap> tilemap;
	std::string scene_file = "scene_unnamed";
	sf::Sprite sprite_bg;

	MegaSprite* follow_sprite = nullptr;

	sf::Vector2f view_pos;

	void edit_mode(bool is_edit)
	{
		m_edit = is_edit;
		m_highlighted_pos = 0;
	}

	sf::View view;
	sf::RenderTexture renderscene;

private:

	bool m_edit = false;
	uint m_highlighted_pos = 0;

	float saturation = 1.0f;
	float saturation_defocus = 0.2f;
	float saturation_refocus = 1.0f;

	float value = 1.0f;
	float value_defocus = 0.8f;
	float value_refocus = 1.0f;

	sf::Texture* edit_tile_texture;
	sf::Shader* shader;

	bool m_focus = true;
	Asset& asset;
	Dialogue& dialogue;
	Persist& persist;
};

#endif // _SCENE_HPP_
