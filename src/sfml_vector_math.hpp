#ifndef _SFML_VECTOR_MATH_HPP_
#define _SFML_VECTOR_MATH_HPP_

namespace sf
{

template<typename T>
sf::Vector2<T> Normalize(const sf::Vector2<T>& source)
{
	T length = sqrt((source.x * source.x) + (source.y * source.y));
	if (length != 0)
		return sf::Vector2<T>(source.x / length, source.y / length);
	else
		return source;
}

}

#endif // _SFML_VECTOR_MATH_HPP_


