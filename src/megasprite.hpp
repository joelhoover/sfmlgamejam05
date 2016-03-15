#ifndef __megasprite_hpp__
#define __megasprite_hpp__

#include <SFML/Graphics.hpp>

#include "sfml_vector_math.hpp"

#include <vector>

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
			if (distance > 2)
			{
				move( sf::Normalize(target_pos - current_pos) * move_speed);
			}
			else
			{
				grid_path.pop_back();
			}
		}
	}

	std::vector<sf::Vector2f> grid_path;
	float move_speed = 0.25f;
	sf::Vector2f pos_last;
	uint speech_id;
	bool use_speech = false;
	uint frames = 1;
};

#endif // __megasprite_hpp__
