#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include "asset.hpp"
#include "persist.hpp"
#include "dialogue.hpp"

class Scene
{

public:

	Scene(Asset& iasset, Dialogue& idialogue, Persist& ipersist)
		:
		asset(iasset),
		dialogue(idialogue),
		persist(ipersist)
	{
	}

private:

	Asset& asset;
	Dialogue& dialogue;
	Persist& persist;
};

#endif // _SCENE_HPP_
