#ifndef _PATHFIND_HPP_
#define _PATHFIND_HPP_

#include <SFML/Graphics.hpp>

#include "sfml_vector_math.hpp"

#include "tilemap.hpp"

#include <iostream>
#include <deque>
#include <vector>
#include <algorithm>

struct PathNode
{

	sf::Vector2i pos;
	uint address;
	PathNode* parent;
	float gcost;
	float hcost;
	float fcost;


	PathNode( const sf::Vector2i& in_pos, PathNode* in_parent, float in_cost, float in_distance)
	 :
	 	pos(in_pos),
	 	parent(in_parent),
	 	gcost(in_cost),
	 	hcost(in_distance),
	 	fcost(gcost + hcost)
	{

	}
};

class PathFind
{

public:

	std::vector<sf::Vector2f> find_path(const sf::Vector2f& in_start_pos, const sf::Vector2f& in_end_pos, TileMap* tilemap)
	{
		sf::Vector2i start_pos = tilemap->scene_to_map_xy(in_start_pos);
		sf::Vector2i end_pos   = tilemap->scene_to_map_xy(in_end_pos);

		if ( tilemap->get(tilemap->map_xy_to_address(end_pos)) ) return {};

		std::deque<std::unique_ptr<PathNode>> list_open;
		std::vector<std::unique_ptr<PathNode>> list_closed;

		float dist = sf::ManhattenDistance(start_pos, end_pos);

		list_open.emplace_back( std::move( std::make_unique<PathNode>( start_pos, nullptr, 0, dist) ) );

		while ( list_open.size() > 0 )
		{
			// sort closest to goal first list_open
			std::sort(list_open.begin(), list_open.end(),
			[] (const std::unique_ptr<PathNode>& a, const std::unique_ptr<PathNode>& b)
			{
				return a->fcost < b->fcost;
			});

			PathNode* current = list_open.front().get();

			if ( current->pos == end_pos )
			{
				std::vector<sf::Vector2f> path;

				//the start tile has a null parent
				while ( current->parent != nullptr )
				{
					path.emplace_back( tilemap->map_xy_to_scene(current->pos));
					current = current->parent;
				}

				list_open.clear();
				list_closed.clear();

				std::cout << path.size() << std::endl;
				return path;
			}

			list_closed.emplace_back(std::move(list_open.front()));
			list_open.erase(list_open.begin());

			// adjacent paths to check

			for ( uint i=0; i<9; ++i )
			{
				// skip current tile
				if (i == 4) continue;

				// TODO work with tile array indices or store Vector2i?
				sf::Vector2i cur_pos = current->pos;

				int xi = (i%3) - 1;
				int yi = (i/3) - 1;
				sf::Vector2i adj_pos = tilemap->clean_map_xy(cur_pos + sf::Vector2i(xi, yi) );
				// skip blocked tile
				if ( tilemap->get( tilemap->map_xy_to_address(adj_pos)))
				{
					continue;
				}

				float gcost = current->gcost + sf::ManhattenDistance(cur_pos, adj_pos);
				float hcost = sf::ManhattenDistance(adj_pos, end_pos);

				// skips tiles already closed (in closed list)
				if (vector_in_list(list_closed, adj_pos) && gcost >= current->gcost)
				{
					continue;
				}

				if ( !vector_in_list(list_open, adj_pos) || gcost < current->gcost)
				{
					list_open.push_back(std::move( std::make_unique<PathNode>(adj_pos, current, gcost, hcost) ));
				}
			}
		}
		list_closed.clear();
		return {};
	}

	template<typename T>
	static bool vector_in_list(const T& list, const sf::Vector2i& pos)
	{
		for ( auto& node : list )
		{
			if ( node->pos == pos ) return true;
		}
		return false;
	}

	/*
	static bool address_in_list(const std::vector<std::unique_ptr<PathNode>>& list, uint address)
	{
		for ( auto& node : list )
		{
			if ( node->address == address ) return true;
		}
		return false;
	}
	*/
};

#endif // _PATHFIND_HPP_
