#ifndef __megasprite_hpp__
#define __megasprite_hpp__

#include <SFML/Graphics.hpp>

#include "sfml_vector_math.hpp"

#include <cmath>
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

	std::unordered_map<std::string, std::vector<sf::IntRect> > frame_rects;
	float frame = 0;
};

#endif // __megasprite_hpp__
