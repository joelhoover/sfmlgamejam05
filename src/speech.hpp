#ifndef _SPEECH_HPP_
#define _SPEECH_HPP_



#include <string>
#include <vector>
#include <unordered_set>

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
		if (obj.count("id"))
			id = obj["id"];

		if (obj.count("text"))
			text = obj["text"];

		if (obj.count("reply"))
			for (auto& element : obj["reply"])
				replies.push_back(element.get<SpeechID>());

		if (obj.count("flag"))
			for (auto& element : obj["flag"])
				state_flags.emplace(element.get<std::string>());
	}

	json save()
	{
		json obj = {{"id", id}, {"text",text} };

		for ( auto elem : replies )
			obj["reply"].push_back(elem);

		for ( auto elem : state_flags )
			obj["flag"].push_back(elem);

		return obj;
	}

	SpeechID id;
	std::string text;
	std::vector<SpeechID> replies;
	std::unordered_set<std::string> state_flags;
};

#endif // _SPEECH_HPP_
