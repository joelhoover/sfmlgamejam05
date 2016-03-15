#ifndef __megasprite_hpp__
#define __megasprite_hpp__

#include <SFML/Graphics.hpp>

#include "sfml_vector_math.hpp"

#include <vector>
#include <string>
#include <unordered_map>

class MegaSprite : public sf::Sprite
{

public:

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
			if (distance > 1)
			{
				move( sf::Normalize(target_pos - current_pos) * move_speed);
			}
			else
			{
				grid_path.pop_back();
			}

			// walk anim
			if (diff_pos.x < 0)
				animate("walk_left");
			else
				animate("walk_right");
		}
		else
		{
			// idle anim
			animate("idle");
		}
	}

	void animate(const std::string& anim_name)
	{
		if ( frame_rects.find( anim_name ) == frame_rects.end() ) return;

		if ( frame_rects[anim_name].size() )
		{

			frame += anim_speed;
			if (frame > frame_rects[anim_name].size()) frame = static_cast<uint>(frame) % frame_rects[anim_name].size();

			setTextureRect( frame_rects[anim_name][ static_cast<uint>(frame) ] );
		}
	}

	std::vector<sf::Vector2f> grid_path;
	float anim_speed = 0.07f;
	float move_speed = 0.666f;
	sf::Vector2f pos_last;
	uint speech_id;
	bool use_speech = false;

	std::unordered_map<std::string, std::vector<sf::IntRect> > frame_rects;
	float frame = 0;
};

#endif // __megasprite_hpp__
