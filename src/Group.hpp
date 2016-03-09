#ifndef _SF_GROUP_HPP_
#define _SF_GROUP_HPP_

#include <SFML/Graphics.hpp>

namespace sf
{

class Group : public sf::Drawable, public std::vector<std::unique_ptr<sf::Drawable>>
{

public:

	Group() : sf::Drawable(), std::vector<std::unique_ptr<sf::Drawable>>()
	{}

	~Group() { clear(); };

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		for(auto i = begin(); i != end(); ++i)
		{
			target.draw(**i, states);
		}
	}
};

//template<typename T>
}

#endif
