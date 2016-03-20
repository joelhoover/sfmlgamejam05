#ifndef _PERSIST_HPP_
#define _PERSIST_HPP_

#include "common.hpp"


struct Persist
{
	std::string get_current_scene_path()
	{
		return "data/"+scene_prefix+"_scene.txt";
	}
	std::string get_current_dialogue_path()
	{
		return "data/"+scene_prefix+"_dialogue.txt";
	}

	std::string scene_prefix = "start"; // prefix for dialogue, scene and background files
	std::vector<uint> dialogue_history;
	std::set<std::string> state_flags;
};

#endif // _PERSIST_HPP_
