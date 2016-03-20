#ifndef __megasprite_hpp__
#define __megasprite_hpp__

#include <SFML/Graphics.hpp>

#include "sfml_vector_math.hpp"

#include <cmath>
#include <vector>
#include <string>
#include <unordered_map>

#include "json.hpp"
using json = nlohmann::json;

class MegaSprite : public sf::Sprite
{

public:

	MegaSprite(const json& obj, sf::Vector2f pos)
	{
		setPosition(pos);
		load(obj);
	}

	void load(const json& obj)
	{
		if (obj.count("name"))
		{
			name = obj["name"].get<std::string>();
		}

		if (obj.count("speech_id"))
		{
			speech_id = obj["speech_id"].get<uint>();
			use_speech = true;
		}

		if ( name == "player" )
		{
			setTextureRect( {0,0,64,64} );
			setOrigin(32,64);
			setScale(0.45f,0.45f);

			frame_rects.emplace( "walk_left_up",   std::vector<sf::IntRect>() );
			frame_rects.emplace( "walk_left_down", std::vector<sf::IntRect>() );
			frame_rects.emplace( "walk_right_up",  std::vector<sf::IntRect>() );
			frame_rects.emplace( "walk_right_down",  std::vector<sf::IntRect>() );
			frame_rects.emplace( "idle",           std::vector<sf::IntRect>() );

			frame_rects["walk_left_up"    ] = { {0,   0+1,  64,  64-1}, {64,   0+1, 64,  64-1}, {128,   0+1, 64,  64-1}, {192,   0+1, 64, 64-1} };
			frame_rects["walk_left_down"  ] = { {0,  64+1,  64,  64-1}, {64,  64+1, 64,  64-1}, {128,  64+1, 64,  64-1}, {192,  64+1, 64, 64-1} };
			frame_rects["walk_right_up"   ] = { {0, 128+1,  64,  64-1}, {64, 128+1, 64,  64-1}, {128, 128+1, 64,  64-1}, {192, 128+1, 64, 64-1} };
			frame_rects["walk_right_down" ] = { {0, 192+1,  64,  64-1}, {64, 192+1, 64,  64-1}, {128, 192+1, 64,  64-1}, {192, 192+1, 64, 64-1} };
			frame_rects["idle"            ] = { {0, 256+1,  64,  64-1}, {64, 256+1, 64,  64-1}, {128, 256+1, 64,  64-1}, {192, 256+1, 64, 64-1} };
		}
	}

	json save()
	{
		json obj;
		obj["name"] = name;

		if ( use_speech )
			obj["speech_id"] = speech_id;

		return obj;
	}

	virtual void update()
	{
		if ( grid_path.size() )
		{
			// follow next path point
			auto current_pos = getPosition();
			auto target_pos  = grid_path.back();
			target_pos.x += 4;
			target_pos.y += 4;

			auto diff_pos = target_pos - current_pos;

			float distance = sf::ManhattenDistance(target_pos, current_pos);
			if (distance > WALK_DISTANCE)
			{
				move( sf::Normalize(target_pos - current_pos) * move_speed);
			}
			else
			{
				grid_path.pop_back();
			}

			// walk anim
			if (diff_pos.x < 0)
				if (diff_pos.y < 0)
					animate(frame_rects["walk_left_up"]);
				else
					animate(frame_rects["walk_left_down"]);
			else
				if (diff_pos.y < 0)
					animate(frame_rects["walk_right_up"]);
				else
					animate(frame_rects["walk_right_down"]);
		}
		else
		{
			// idle anim
			animate(frame_rects["idle"]);
		}
	}

	void animate(std::vector<sf::IntRect>& rects)
	{
		if ( rects.size() )
		{

			frame = std::fmod(frame + anim_speed, rects.size());

			setTextureRect( rects[ static_cast<uint>(frame) ] );
		}
	}

	std::vector<sf::Vector2f> grid_path;
	float anim_speed = 0.07f;
	float move_speed = 0.666f;
	sf::Vector2f pos_last;
	uint speech_id;
	bool use_speech = false;

	std::string name; // name for recognising sprite

	std::unordered_map<std::string, std::vector<sf::IntRect> > frame_rects;
	float frame = 0;
};

#endif // __megasprite_hpp__
