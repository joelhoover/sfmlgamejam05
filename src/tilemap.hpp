#ifndef __TILE_MAP_HPP__
#define __TILE_MAP_HPP__

#include <cmath>

class TileMap
{

public:

	TileMap( uint in_map_width, uint in_tile_size )
	:
		map_width(in_map_width),
		map_area(map_width*map_width),
		tile_width(in_tile_size),
		tile_shift( std::log2(tile_width) )
	{
	}

	bool get(uint apos) const
	{
		return grid[apos];
	}

	void set(uint apos, bool new_val)
	{
		grid[apos] = new_val;
	}

	void toggle(uint apos)
	{
		grid[apos] = !grid[apos];
	}

	sf::Vector2i scene_to_map_xy(const sf::Vector2f& pos) const
	{
		return sf::Vector2i( static_cast<uint>(pos.x) >> tile_shift, static_cast<uint>(pos.y) >> tile_shift);
	}

	sf::Vector2i clean_map_xy(sf::Vector2i pos) const
	{
		// constrain to size limits
		pos.x = (pos.x < 0) ? 0 : (pos.x < map_width) ? pos.x : map_width;
		pos.y = (pos.y < 0) ? 0 : (pos.y < map_width) ? pos.y : map_width;
		return pos;
	}

	uint map_xy_to_address(sf::Vector2i pos) const
	{
		return pos.x + pos.y * map_width;
	}

	uint scene_to_address( const sf::Vector2f& pos) const
	{
		return map_xy_to_address(clean_map_xy(scene_to_map_xy(pos)));
	}

	sf::Vector2i address_to_map_xy( uint addr ) const
	{
		return sf::Vector2i( addr%map_width, addr/map_width );
	}

	sf::Vector2f map_xy_to_scene( const sf::Vector2i& pos ) const
	{
		return sf::Vector2f( pos.x << tile_shift, pos.y << tile_shift);
	}

	sf::Vector2f address_to_scene( uint addr ) const
	{
		return map_xy_to_scene( address_to_map_xy(addr) );
	}

	uint map_width = 128;
	uint map_area = 1024;
	uint tile_width = 8;
	uint tile_shift = 3;

	std::vector<bool> grid;

};

#endif // __TILE_MAP_HPP__
