#ifndef _PATHFIND_HPP_
#define _PATHFIND_HPP_

#include <SFML/Graphics.hpp>

#include "sfml_vector_math.hpp"

#include "tilemap.hpp"

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

	std::vector<uint> find_path(TileMap* tilemap)
	{
		list_open.clear();
		list_closed.clear();

		//float dist = sf::ManhattenDistance(start_pos, end_pos);
		//float dist = sf::ManhattenDistance(tilemap->map_xy_to_scene(start_pos), tilemap->map_xy_to_scene(end_pos));
		//float dist = sf::ManhattenDistance(tilemap->map_xy_to_address(start_pos), tilemap->map_xy_to_address(end_pos));

		float dist = sf::ManhattenDistance(start_pos, end_pos);

		list_open.emplace_back( std::move( std::make_unique<PathNode>( start_pos, nullptr, 0, dist) ) );

		while ( list_open.size() > 0 )
		{
			// sort closest to goal first list_open
			std::sort(list_open.begin(), list_open.end(),
			[] (const std::unique_ptr<PathNode>& a, const std::unique_ptr<PathNode>& b)
			{
				return b->fcost < a->fcost;
			});

			PathNode* current = list_open.front().get();

			if ( current->pos == end_pos )
			{
				std::vector<uint> path;
				while ( current->parent != nullptr )
				{
					//path.emplace_back( tilemap->scene_to_address(current->pos) );
					path.emplace_back( current->address );

					current = current->parent; //only start tile is null parent
				}

				list_open.clear();
				list_closed.clear();
				return path;
			}

			list_closed.emplace_back(std::move(list_open.front()));
			list_open.erase(list_open.begin());

			// adjacent paths to check

			for ( uint i=0; i<9; ++i )
			{
				if (i == 4) continue;

				// TODO work with tile array indices or store Vector2i?
				// uint current_index = current->pos;
				// auto cur_pos = tilemap->address_to_scene(current->pos)
				// auto cur_pos = tilemap->map_xy_to_scene(current->pos);

				sf::Vector2i cur_pos = current->pos;

				int xi = (i%3) - 1;
				int yi = (i/3) - 1;

				sf::Vector2i adj_pos = cur_pos + sf::Vector2i(xi, yi);

				if ( tilemap->get(tilemap->map_xy_to_address(adj_pos)) ) continue;

				//if ( tilemap->get(tilemap->map_xy_to_address(adj_pos))) continue;
				//if (adj == nullptr) continue;
				//if (adj->is_collideable()) continue;

				float gcost = current->gcost + sf::ManhattenDistance(cur_pos, adj_pos);
				float hcost = sf::ManhattenDistance(adj_pos, end_pos);

				std::unique_ptr<PathNode> node = std::make_unique<PathNode>(adj_pos, current, gcost, hcost);

				// TODO consider address_in_list instead
				if (vector_in_list(list_closed, adj_pos) && gcost >= current->gcost) // or node->gcost ?
				{
					continue;
				}

				if ( !vector_in_list(list_open, adj_pos) || gcost < current->gcost) // or node->gcost ?
				{
					list_open.emplace_back( std::move(node) );
				}

			}
			// skips current tile
			// skips blocked tiles
			// skips tiles already visited (in closed list)
		}

		list_closed.clear();
		return {};
	}

	static bool vector_in_list(const std::vector<std::unique_ptr<PathNode>>& list, const sf::Vector2i& pos)
	{
		for ( auto& node : list )
		{
			if ( node->pos == pos ) return true;
		}
		return false;
	}

	static bool address_in_list(const std::vector<std::unique_ptr<PathNode>>& list, uint address)
	{
		for ( auto& node : list )
		{
			if ( node->address == address ) return true;
		}
		return false;
	}

	//TileMap* tilemap;
	//PathNode* current;
	//uint start_address;
	//uint end_address;

	sf::Vector2i start_pos;
	sf::Vector2i end_pos;

	std::vector<std::unique_ptr<PathNode>> list_open;
	std::vector<std::unique_ptr<PathNode>> list_closed;

};

#endif // _PATHFIND_HPP_
