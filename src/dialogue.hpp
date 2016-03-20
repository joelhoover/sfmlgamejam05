#ifndef _DIALOGUE_HPP_
#define _DIALOGUE_HPP_

#include <iostream>
#include <string>
#include <unordered_set>
#include <algorithm>

#include "common.hpp"

#include "json.hpp"
#include "load_json.hpp"
#include "save_text.hpp"

#include "speech.hpp"
#include "Group.hpp"

#include "persist.hpp"

using json = nlohmann::json;

// determine choice
// no entry - end of dialogue
// 1 entry - silence, to continue a monologue
// 1 entry - a pre-determined reply ( no choice )

class Dialogue : public sf::Drawable
{

public:

	Dialogue(Asset& iasset, Persist& ipersist)
		:
		asset(iasset),
		persist(ipersist)
	{
		cursor.setTexture( *asset.textures["text_cursor"] );
		//cursor.setOrigin(0,5);
		cursor.setColor(sf::Color::Red);
	}

	void load_from_file(const std::string& file_name)
	{
		json data = shrapx::load_json(file_name);
		for (auto& element : data)
		{
			entries.emplace( element["id"].get<SpeechID>(), std::make_unique<Speech>(element) );
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
		shrapx::save_text(file_name, data.dump(1));
	}

	void update()
	{
		update_text();
	}

	void mouse_selected(const sf::Vector2f& pos)
	{
		m_highlighted_line = contain_pos(pos);

		edit_cursor_fix_limits();

		reset_text();
	}

	uint contain_pos(const sf::Vector2f& pos)
	{
		for(uint i=0; i<texts.size(); ++i)
		{
			sf::Text* text = static_cast<sf::Text*>( texts[i].get() );

			assert(text != nullptr);

			if ( text->getGlobalBounds().contains(pos) )
			{
				return i;
			}
		}
		return texts.size();
	}

	uint closest_y(float pos_y)
	{
		uint closest_i = texts.size();
		float closest_diff = 1000.0f;
		for(uint i=0; i<texts.size(); ++i)
		{
			sf::Text* text = static_cast<sf::Text*>( texts[i].get() );

			assert(text != nullptr);

			auto bounds = text->getGlobalBounds();
			float dist_y = fabs(pos_y - (bounds.top+(bounds.height*0.5f)));

			if ( dist_y < closest_diff )
			{
				closest_diff = dist_y;
				closest_i = i;
			}
		}
		//if (closest_diff == 1000.0f) return 0;
		return closest_i;
	}

	void activate(uint selection)
	{
		assert( entries.find(selection) != entries.end() );

		m_active = true;
		m_active_speech_id = selection;
		m_text_fill_counter = 0;
		m_text_fill_complete = false;

		m_highlighted_line = m_edit ? 0 : 1;

		edit_cursor_place_at_end();

		reset_text();
	}

	void activate_highlighted_or_default()
	{
		if ( !m_text_fill_complete ) return;

		Speech* speech = get_entry();
		if (speech == nullptr)
		{
			deactivate();
			return;
		}

		// default to 1 reply if 1 reply
		if (speech->replies.size() == 1)
		{
			m_highlighted_line = 1;
		}

		if ( speech->replies.size() == 0 || m_highlighted_line == 0 )
		{
			deactivate();
			return;
		}

		Speech* entrep = get_entry( m_highlighted_line );
		if (entrep == nullptr) return;

		apply_state_flags(entrep);

		activate( entrep->id );
	}

	void apply_state_flags(Speech* speech)
	{
		persist.state_flags.insert(speech->state_flags.begin(), speech->state_flags.end());
		/*std::cout << "flags: ";
		for (auto& str : persist.state_flags )
			std::cout << str << ", ";
		std::cout << std::endl << std::endl;*/
	}

	Speech* get_entry(uint reply_id=0)
	{
		auto entry = entries[m_active_speech_id].get();

		if (entry == nullptr) return nullptr;
		if (reply_id == 0) return entry;

		if ( reply_id > entry->replies.size() ) return nullptr;

		return entries[entry->replies[reply_id-1]].get();
	}

	void reset_text()
	{
		Speech* selected = entries[m_active_speech_id].get();
		if (selected == nullptr) return;

		texts.clear();

		{
			std::unique_ptr<sf::Text> text = std::make_unique<sf::Text>();
			texts.emplace_back(std::move(text));

			std::unique_ptr<sf::Sprite> spr = std::make_unique<sf::Sprite>();
			spr->setTexture(*asset.textures["box"]);
		}

		if ( selected->replies.size() )
		{
			for ( uint text_id=0; text_id<(selected->replies.size()); ++text_id )
			{
				std::unique_ptr<sf::Text> text = std::make_unique<sf::Text>();
				texts.emplace_back(std::move(text));

				std::unique_ptr<sf::Sprite> spr = std::make_unique<sf::Sprite>();
				spr->setTexture(*asset.textures["box"]);
			}
		}
	}

	void update_text()
	{
		Speech* selected = entries[m_active_speech_id].get();
		if (selected == nullptr) return;
		if ( texts.size() == 0 ) return;
		uint pos_x = 60;
		uint pos_y = 45;
		// speech text
		{
			std::string str = selected->text;

			sf::Text* text = static_cast<sf::Text*>( texts.front().get() );


			assert(text != nullptr);

			if ( m_edit && m_highlighted_line == 0 )
			{
				set_selected(text);
			}
			else
			{
				uint strsize = str.size();
				if (m_text_fill_counter < strsize)
				{
					str = str.substr(0, m_text_fill_counter);
					m_text_fill_counter += m_text_fill_rate;
					if (m_text_fill_counter >= strsize) m_text_fill_complete = true;
				}
				set_deselected(text);
			}

			text->setString( str );

			if ( m_edit && m_highlighted_line == 0 )
			{
				cursor.setPosition( text->findCharacterPos(m_cursor_position) );
			}

			text->setPosition(pos_x, pos_y);

			//sf::Sprite* spr = static_cast<sf::Sprite*>(shapes_debug.front().get());
			//spr->setScale(text->getGlobalBounds().width/16.0f, text->getGlobalBounds().height/16.0f);
			//spr->setPosition( text->getPosition());
		}

		// reply list
		if ( selected->replies.size() == 0 ) return;

		// start height: center of screen
		pos_y = 15 + (ZOOM_H/2);

		for ( uint text_id=1; text_id<(selected->replies.size()+1); ++text_id )
		{
			sf::Text* text = static_cast<sf::Text*>( texts[text_id].get() );

			assert(text != nullptr);

			std::string str = entries[selected->replies[text_id-1]]->text;

			if (text_id == m_highlighted_line)
				set_selected(text);
			else
				set_deselected(text);

			text->setString( "> " + str );

			if ( m_edit && (text_id == m_highlighted_line))
			{
				cursor.setPosition( text->findCharacterPos(m_cursor_position+2) );
			}

			text->setPosition(pos_x, pos_y);
			pos_y += text->getGlobalBounds().height+10;

			//sf::Sprite* spr = static_cast<sf::Sprite*>(shapes_debug[text_id].get());
			//spr->setScale(text->getGlobalBounds().width/16.0f, text->getGlobalBounds().height/16.0f);
			//spr->setPosition( text->getPosition());
		}
	}

	// editor settings
	SpeechID add_speech()
	{
		auto new_entry = std::make_unique<Speech>();
		SpeechID new_id = speech_counter;
		new_entry->id = new_id;
		new_entry->text = "new";
		entries[new_id] = std::move(new_entry);
		speech_counter++;

		return new_id;
	}

	SpeechID add_reply(Speech* entry)
	{
		//auto entry = entries[m_active_speech_id].get();
		//assert(entry != nullptr);
		//if (entry == nullptr ) return;

		SpeechID new_id = add_speech();

		entry->replies.push_back(new_id);

		return new_id;
	}


	void edit_mode(bool is_edit)
	{
		m_edit = is_edit;
		m_highlighted_line = 0;
		edit_cursor_place_at_end();
		reset_text();
	}

	void edit_up()
	{
		if ( m_highlighted_line > 0 ) m_highlighted_line--;

		edit_cursor_fix_limits();
	}

	void edit_down()
	{
		m_highlighted_line++;

		auto entry = entries[m_active_speech_id].get();
		if (entry == nullptr ) return;
		if ( m_highlighted_line > entry->replies.size() )
		{
			add_reply(entry);
		}

		edit_cursor_fix_limits();

		reset_text();
	}

	void edit_left()
	{
		if (m_cursor_position == 0) return;
		--m_cursor_position;

		edit_cursor_fix_limits();
	}

	void edit_right()
	{
		Speech* entrep = get_entry(m_highlighted_line);
		if (entrep == nullptr) return;
		if (m_cursor_position >= entrep->text.size()) return;
		++m_cursor_position;
	}

	void edit_backspace()
	{
		if (m_cursor_position > 0 )
		{
			--m_cursor_position;

			Speech* entrep = get_entry(m_highlighted_line);
			if (entrep == nullptr) return;
			entrep->text.erase(m_cursor_position,1);
		}
	}

	void edit_insert(char c)
	{
		Speech* entrep = get_entry(m_highlighted_line);
		if (entrep == nullptr) return;
		entrep->text.insert(m_cursor_position,1,c);
		m_cursor_position++;
	}

	void edit_newline()
	{
		edit_insert('\n');
	}

	void edit_cursor_fix_limits()
	{
		Speech* entrep = get_entry(m_highlighted_line);
		if(entrep == nullptr) return;
		if (m_cursor_position > entrep->text.size()) m_cursor_position = entrep->text.size();
	}

	void edit_cursor_place_at_end()
	{
		Speech* entrep = get_entry(m_highlighted_line);
		if(entrep == nullptr) return;
		m_cursor_position = entrep->text.size();
	}

	void set_selected(sf::Text* text)
	{
		text->setFont(*default_font);
		text->setCharacterSize(char_size);

		if (m_edit)
			text->setColor( sf::Color(255,160,127,255) );
		else
			text->setColor( sf::Color(127,160,255,255) );

		//text->setFillColor( sf::Color(127,127,127,255) );
		//text->setOutlineColor( sf::Color(255,127,127,255) );
		//text->setOutlineThickness(2);
	}

	void set_deselected(sf::Text* text)
	{
		text->setFont(*default_font);
		text->setCharacterSize(char_size);
		//text->setColor( sf::Color(127,127,127,255) );
		text->setColor( sf::Color(160,192,192,255) );
		//text->setFillColor( sf::Color(127,127,127,255) );
		//text->setOutlineColor( sf::Color(255,127,127,255) );
		//text->setOutlineThickness(0);
	}


	uint speech_counter = 0;
	std::unordered_map<SpeechID, std::unique_ptr<Speech>> entries;
	std::unordered_map<SpeechID, std::string> state_ids;


	// sprite stuff

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		//target.draw(shapes_debug, states);
		target.draw(texts, states);

		if (m_edit) target.draw(cursor, states);
	}

	sf::Font* default_font;
	uint char_size = 10;

	bool is_active() { return m_active; }
	void deactivate() { m_active = false; }

private:

	float m_text_fill_counter = 0;
	float m_text_fill_rate = 0.25f;
	bool m_text_fill_complete = false;

	uint m_active_speech_id = 0;
	uint m_highlighted_line = 0;
	uint m_cursor_position = 0;

	bool m_edit = false;
	bool m_active = false;

	sf::Sprite cursor;

	sf::Group texts;
	//sf::Group shapes_debug;

	Asset& asset;
	Persist& persist;

};

#endif // _DIALOGUE_HPP_
