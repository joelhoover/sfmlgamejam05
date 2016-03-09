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
			//entries.emplace_back( std::make_unique<Speech>(element) );
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

	void advance(uint selection)
	{
		selected_line = 0;
		//if ( selection >= entries.size() )
		if ( entries[selection] == nullptr )
		{
			if (edit)
			{
				// make a new entry, connect as response of previous
				// toggle from self to other

				entries[speech_counter] = std::make_unique<Speech>();
				selection = speech_counter;
				speech_counter++;
				//entries.emplace( std::make_unique<Speech>() );

			}
			else
			{
				// end of dialogue
				active = false;
				//displayed_entry = 0;
				speech = nullptr;
				return;
			}
		}

		//std::cout << get_entry()->print() << std::endl;
		//std::cout << get_entry()->print_choices() << std::endl;

		active = true;
		//displayed_entry = selection;
		//speech = get_entry(selection);
		speech = entries[selection].get();

		update_sprite();
	}

	Speech* get_entry_reply(uint num)
	{
		if (speech == nullptr ||
				num >= speech->replies.size() )
			return nullptr;

		return entries[speech->replies[num]].get();
	}

	uint text_selected(const sf::Vector2f& pos)
	{
		uint chosen = 0;
		for(auto i = texts.begin(); i != texts.end(); ++i)
		{
			sf::Text* f = static_cast<sf::Text*>( i->get() );

			if (f->getGlobalBounds().contains(pos)) return chosen;

			++chosen;
		}
		return 0;
	}

	void mouse_selected(const sf::Vector2f& pos)
	{
		uint text_num = text_selected(pos);
		if ( text_num == 0) return;

		selected_line = text_num;

		std::cout << selected_line << std::endl;

		fix_cursor_limits();
		update_sprite();
	}

	void advance_selected()
	{
		Speech* entrep = get_entry_reply(selected_line);
		if (entrep == nullptr) return;

		advance( entrep->id );
	}

	// editor settings

	void edit_up()
	{
		if ( selected_line > 0 ) selected_line--;
		Speech* entrep = get_entry_reply(selected_line);
		if (entrep == nullptr) return;

		fix_cursor_limits();
		update_sprite();
	}

	void edit_down()
	{
		selected_line++;

		if ( selected_line == speech->replies.size() )
		{
			entries[speech_counter] = std::make_unique<Speech>();

			speech->replies.push_back(speech_counter);
			speech_counter++;
		}

		fix_cursor_limits();
		update_sprite();
	}

	void edit_left()
	{
		if (cursor_pos == 0) return;
		--cursor_pos;

		fix_cursor_limits();

		update_sprite();
	}

	void edit_right()
	{
		Speech* entrep = get_entry_reply(selected_line);
		if (entrep == nullptr) return;
		if (cursor_pos >= entrep->text.size()) return;
		++cursor_pos;
		update_sprite();
	}

	void fix_cursor_limits()
	{
		Speech* entrep = get_entry_reply(selected_line);
		if (entrep == nullptr) return;
		if (cursor_pos > entrep->text.size()) cursor_pos = entrep->text.size();
	}

	void edit_backspace()
	{
		if (cursor_pos > 0 )
		{
			--cursor_pos;
			Speech* entrep = get_entry_reply(selected_line);
			if (entrep == nullptr) return;
			entrep->text.erase(cursor_pos,1);
		}
		update_sprite();
	}

	void edit_insert(char c)
	{
		Speech* entrep = get_entry_reply(selected_line);
		if (entrep == nullptr) return;
		entrep->text.insert(cursor_pos,1,c);
		cursor_pos++;
		update_sprite();
	}

	void edit_newline()
	{
		edit_insert('\n');
	}

	void set_selected(sf::Text* text)
	{
		text->setColor( sf::Color(255,127,127,255) );
		//text->setFillColor( sf::Color(127,127,127,255) );
		//text->setOutlineColor( sf::Color(255,127,127,255) );
		//text->setOutlineThickness(2);
	}

	void set_deselected(sf::Text* text)
	{
		text->setColor( sf::Color(127,127,127,255) );
		//text->setFillColor( sf::Color(127,127,127,255) );
		//text->setOutlineColor( sf::Color(255,127,127,255) );
		//text->setOutlineThickness(0);
	}

	void update_sprite()
	{
		if (speech == nullptr) return;

		texts.clear();

		uint pos_y = 0;

		std::unique_ptr<sf::Text> text = std::make_unique<sf::Text>();
		text->setFont(*default_font);
		text->setCharacterSize(char_size);


		std::string stext = speech->text;

		if (edit && selected_line == 0)
		{
			stext.insert(cursor_pos,"|",1);
			set_selected(text.get());
		}
		else
		{
			set_deselected(text.get());
		}

		text->setString(stext + '\n');

		text->setPosition(0, 0);
		pos_y += text->getGlobalBounds().height+1;

		texts.emplace_back( std::move(text) );

		for ( uint i=0; i<speech->replies.size(); ++i)
		{
			std::unique_ptr<sf::Text> tex = std::make_unique<sf::Text>();
			tex->setFont(*default_font);
			tex->setCharacterSize(char_size);


			Speech* entrep = get_entry_reply(i);
			if (entrep == nullptr) continue;
			stext = entrep->text;

			if (edit && selected_line == i)
			{
				stext.insert(cursor_pos,"|");
				set_selected(tex.get());
			}
			else
			{
				set_deselected(tex.get());
			}

			tex->setString( "> " + stext + '\n' );

			tex->setPosition(0, pos_y);
			pos_y += tex->getGlobalBounds().height+1;

			texts.emplace_back( std::move(tex) );
		}

		// sprite layout
		/*uint pos_y = 0;
		for(auto i = texts.begin(); i != texts.end(); ++i)
		{
			sf::Text* f = static_cast<sf::Text*>( i->get() );
			f->setPosition(0,pos_y);

			pos_y += f->getGlobalBounds().height+1;
		}*/
	}

	//Sprite side

	void resize(uint new_size)
	{
		texts.resize(new_size);
		for(auto i = texts.begin(); i != texts.end(); ++i)
		{
			*i =  std::make_unique<sf::Text>();
		}
	}

	void set_font(const sf::Font& font)
	{
		for(auto i = texts.begin(); i != texts.end(); ++i)
		{
			sf::Text* f = static_cast<sf::Text*>( i->get() );
			f->setFont(font);
			f->setCharacterSize(char_size);
		}
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(texts, states);
	}

	// determine choice
	// no entry - end of dialogue
	// 1 entry - silence, to continue a monologue
	// 1 entry - a pre-determined reply ( no choice )
	uint speech_counter = 0;
	std::unordered_map<SpeechID, std::unique_ptr<Speech>> entries;
	std::unordered_map<SpeechID, std::string> state_ids;


	// editor state

	bool edit = false;
	uint line_count = 0;
	uint selected_line = 0;
	uint cursor_pos = 0;

	// spritey stuff

	// display + using?
	bool active = false;

	sf::Font* default_font;
	uint char_size = 15;

	sf::Group texts;
	Speech* speech = nullptr;

private:

	Asset& asset;

};

#endif // _DIALOGUE_HPP_
