#ifndef _DIALOGUE_HPP_
#define _DIALOGUE_HPP_

#include <iostream>

#include "common.hpp"

#include "json.hpp"
#include "load_json.hpp"
#include "save_text.hpp"

#include "speech.hpp"
#include "Group.hpp"

using json = nlohmann::json;

// determine choice
// no entry - end of dialogue
// 1 entry - silence, to continue a monologue
// 1 entry - a pre-determined reply ( no choice )

class Dialogue : public sf::Transformable, public sf::Drawable
{

public:

	Dialogue(Asset& iasset)
		:
		asset(iasset)
	{
	}

	void load_from_file(const std::string& file_name)
	{
		json data = shrapx::load_json(file_name);
		for (auto& element : data)
		{
			entries.emplace( element[0].get<SpeechID>(), std::make_unique<Speech>(element) );
			speech_counter++;
		}
	}

	void save_to_file(const std::string& file_name)
	{
		json data = json::array();
		for (auto& entry_pair : entries)
		{
			data.push_back( entry_pair.second->save() );
		}
		shrapx::save_text(file_name, data.dump());
	}

	void mouse_selected(const sf::Vector2f& pos)
	{
		uint text_num;

		bool picked = false;
		for(uint i=0; i<texts.size(); ++i)
		{
			sf::Text* text = static_cast<sf::Text*>( texts[i].get() );

			assert(text != nullptr);

			if (text->getGlobalBounds().contains(pos))
			{
				picked = true;
				text_num = i;
			}
		}

		if ( !picked )
		{
			std::cout << "!" << std::endl;
			return;
		}
		else
		{
			std::cout << text_num << std::endl;
		}

		edit_line_position = text_num;

		fix_cursor_limits();

		//reset_text();
		update_text();
	}

	void advance(uint selection)
	{
		edit_line_position = 0;
		edit_cursor_position = 0;

		if ( entries[selection] == nullptr )
		{
			if (edit)
			{
				// make a new entry, connect as response of previous
				// toggle from self to other

				entries[speech_counter] = std::make_unique<Speech>();
				selection = speech_counter;
				speech_counter++;
			}
			else
			{
				// end of dialogue
				active = false;
				view_speech_id = 0;
				return;
			}
		}

		//std::cout << get_entry()->print() << std::endl;
		//std::cout << get_entry()->print_choices() << std::endl;

		active = true;

		reset_text();
		update_text();
	}

	Speech* get_entry(uint reply_id=0)
	{
		auto entry = entries[view_speech_id].get();

		if (entry == nullptr) return nullptr;
		if (reply_id == 0) return entry;

		if ( reply_id > entry->replies.size() ) return nullptr;

		return entries[entry->replies[reply_id-1]].get();
	}

	void reset_text()
	{
		Speech* selected = entries[view_speech_id].get();
		if (selected == nullptr) return;

		texts.clear();

		for ( uint text_id=0; text_id<(selected->replies.size()+1); ++text_id )
		{
			std::unique_ptr<sf::Text> text = std::make_unique<sf::Text>();
			texts.emplace_back(std::move(text));
		}
	}

	void update_text()
	{
		Speech* selected = entries[view_speech_id].get();
		if (selected == nullptr) return;

		uint pos_y = 0;
		for ( uint text_id=0; text_id<(selected->replies.size()+1); ++text_id )
		{
			sf::Text* text = static_cast<sf::Text*>( texts[text_id].get() );

			assert(text != nullptr);

			std::string str = (text_id==0) ? selected->text : entries[selected->replies[text_id-1]]->text;

			if (edit && text_id == edit_line_position)
			{
				str.insert(edit_cursor_position,"|",1);
				set_selected(text);
			}
			else
			{
				set_deselected(text);
			}

			text->setString( (text_id==0?"":"> ") + str + '\n' );

			text->setPosition(0, pos_y);
			pos_y += text->getGlobalBounds().height+1;

		}
	}
	void advance_selected()
	{
		Speech* entrep = get_entry(edit_line_position);
		if (entrep == nullptr) return;

		advance( entrep->id );
	}

	// editor settings

	void edit_up()
	{
		if ( edit_line_position > 0 ) edit_line_position--;

		fix_cursor_limits();
		//reset_text();
		update_text();
	}

	void edit_down()
	{
		edit_line_position++;

		auto entry = entries[view_speech_id].get();
		if (entry == nullptr ) return;
		if ( edit_line_position > entry->replies.size() )
		{
			entries[speech_counter] = std::make_unique<Speech>();
			entry->replies.push_back(speech_counter);
			speech_counter++;
		}

		fix_cursor_limits();

		reset_text();
		update_text();
	}

	void edit_left()
	{
		if (edit_cursor_position == 0) return;
		--edit_cursor_position;

		fix_cursor_limits();

		//reset_text();
		update_text();
	}

	void edit_right()
	{
		Speech* entrep = get_entry(edit_line_position);
		if (entrep == nullptr) return;
		if (edit_cursor_position >= entrep->text.size()) return;
		++edit_cursor_position;

		//reset_text();
		update_text();
	}

	void fix_cursor_limits()
	{
		Speech* entrep = get_entry(edit_line_position);
		if (entrep == nullptr) return;
		if (edit_cursor_position > entrep->text.size()) edit_cursor_position = entrep->text.size();
	}

	void edit_backspace()
	{
		if (edit_cursor_position > 0 )
		{
			--edit_cursor_position;

			Speech* entrep = get_entry(edit_line_position);
			if (entrep == nullptr) return;
			entrep->text.erase(edit_cursor_position,1);

			//reset_text();
			update_text();
		}
	}

	void edit_insert(char c)
	{
		Speech* entrep = get_entry(edit_line_position);
		if (entrep == nullptr) return;
		entrep->text.insert(edit_cursor_position,1,c);
		edit_cursor_position++;

		//reset_text();
		update_text();
	}

	void edit_newline()
	{
		edit_insert('\n');
	}

	void set_selected(sf::Text* text)
	{
		text->setFont(*default_font);
		text->setCharacterSize(char_size);
		text->setColor( sf::Color(255,127,127,255) );
		//text->setFillColor( sf::Color(127,127,127,255) );
		//text->setOutlineColor( sf::Color(255,127,127,255) );
		//text->setOutlineThickness(2);
	}

	void set_deselected(sf::Text* text)
	{
		text->setFont(*default_font);
		text->setCharacterSize(char_size);
		text->setColor( sf::Color(127,127,127,255) );
		//text->setFillColor( sf::Color(127,127,127,255) );
		//text->setOutlineColor( sf::Color(255,127,127,255) );
		//text->setOutlineThickness(0);
	}


	uint speech_counter = 0;
	std::unordered_map<SpeechID, std::unique_ptr<Speech>> entries;
	std::unordered_map<SpeechID, std::string> state_ids;


	// editor state

	uint view_speech_id = 0;

	bool edit = false;
	uint edit_line_position = 0;
	uint edit_cursor_position = 0;

	// spritey stuff

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(texts, states);
	}

	// display + using?
	bool active = false;

	sf::Font* default_font;
	uint char_size = 15;

	sf::Group texts;

	Speech* speech_viewing = nullptr;

private:

	Asset& asset;

};

#endif // _DIALOGUE_HPP_
