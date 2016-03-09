#ifndef _SPEECH_HPP_
#define _SPEECH_HPP_



#include <string>
#include <vector>

#include "common.hpp"

#include "json.hpp"
using json = nlohmann::json;

struct Speech
{
	Speech() = default;

	Speech( SpeechID iid,	const std::string& itext)
	:
		id(iid),
		text(itext)
	{

	}

	Speech(const json& obj)
	{
		load(obj);
	}

	void load(const json& obj)
	{
		if ( obj.size() == 0 ) return;
		id = obj[0];

		if ( obj.size() == 1 ) return;

		text = obj[1];

		if (obj.size() == 2) return;

		for (auto& element : obj[2])
		{
			replies.push_back( element.get<SpeechID>() );
		}
	}

	json save()
	{
		json obj = {id, text, nullptr};

		for ( SpeechID reply : replies )
		{
			obj[2].push_back( reply );
		}
		return obj;
	}

	SpeechID id;
	std::string text;
	std::vector<SpeechID> replies;
};

#endif // _SPEECH_HPP_
