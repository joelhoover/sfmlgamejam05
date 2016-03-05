#ifndef _DIALOGUE_HPP_
#define _DIALOGUE_HPP_

class Dialogue
{

public:

	Dialogue(Asset& iasset)
		:
		asset(iasset)
	{
	}

private:

	Asset& asset;
};

#endif // _DIALOGUE_HPP_
