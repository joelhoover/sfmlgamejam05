#ifndef _PERSIST_HPP_
#define _PERSIST_HPP_

#include "common.hpp"

struct Persist
{
	std::vector<uint> dialogue_history;
	std::set<std::string> state_flags;
};

#endif // _PERSIST_HPP_
