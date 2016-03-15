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
T Distance(const sf::Vector2<T>& a, const sf::Vector2<T>& b)
{
	T dx = a.x - b.x;
	T dy = a.y - b.y;
	return sqrt( dx * dx  + dy * dy);
}

template<typename T>
T ManhattenDistance(const sf::Vector2<T>& a, const sf::Vector2<T>& b)
{
	T dx = a.x - b.x;
	T dy = a.y - b.y;
	return abs(dx) + abs(dy);
}

template<>
float ManhattenDistance(const sf::Vector2f& a, const sf::Vector2f& b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return fabs(dx) + fabs(dy);
}

}

#endif // _SFML_VECTOR_MATH_HPP_


