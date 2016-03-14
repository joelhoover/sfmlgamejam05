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

template<typename T>
sf::Vector2<T> Distance(const sf::Vector2<T>& a, const sf::Vector2<T>& b)
{
	T dx = a.x - b.x;
	T dy = a.y - b.y;
	return sqrt( dx * dx  + dy * dy);
}

}

#endif // _SFML_VECTOR_MATH_HPP_


