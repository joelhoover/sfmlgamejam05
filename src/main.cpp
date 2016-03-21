#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <cstdint>
#include <iostream>

#include "common.hpp"
#include "asset.hpp"
#include "persist.hpp"
#include "dialogue.hpp"
#include "scene.hpp"



const sf::VideoMode VIDEO_MODE(SCREEN_W, SCREEN_H);

int main()
{

	sf::Event event;
	bool terminate = false;
	sf::Clock frame_clock;
	sf::Time frame_time = sf::Time::Zero;

	sf::RenderWindow window(VIDEO_MODE, "sfmlgamejam05", sf::Style::Close);
	window.setFramerateLimit(60);
	window.setMouseCursorVisible(false);

	sf::RenderTexture renderui;
	renderui.create(ZOOM_W, ZOOM_H, false);

	sf::Sprite renderuisprite;
	renderuisprite.setTexture(renderui.getTexture());
	renderuisprite.setScale( float(ZOOM), float(ZOOM) );

	Persist persist;

	Asset asset;

	// load files here
	asset.load_font("pixel", "data/font/ARCADEPI.TTF");

	asset.load_texture("placeholder", "data/tex/placeholder.png");

	asset.load_texture("player", "data/tex/player.png");

	asset.load_texture("phone", "data/tex/phone.png");

	asset.load_texture("jerry_stretcher", "data/tex/jerry_stretcher.png");
	asset.load_texture("maria", "data/tex/maria.png");

	asset.load_texture("box", "data/tex/box.png");
	asset.load_texture("text_cursor", "data/tex/text_cursor.png");

	asset.load_texture("ui", "data/tex/ui.png");

	asset.load_texture("exit", "data/tex/exit.png");

	asset.load_shader("fader", "data/shaders/fader_v.glsl", "data/shaders/fader_f.glsl");

	// scenes
	asset.load_texture("cop_appt", "data/tex/cop_appt.png");
	asset.load_texture("crimescene", "data/tex/crimescene.png");
	asset.load_texture("police_station", "data/tex/police_station.png");


	sf::Sprite pointer_sprite( *asset.textures["ui"] );
	pointer_sprite.setTextureRect( sf::IntRect(0,0,8,8) );

	uint pointer_type = CURSOR_DEFAULT;

	sf::Sprite edit_mode_sprite( *asset.textures["ui"] );
	edit_mode_sprite.setTextureRect( sf::IntRect(32,0,32,16) );
	edit_mode_sprite.setPosition( ZOOM_W-32, 0 );

	sf::Sprite renderscenesprite;
	renderscenesprite.setScale( float(ZOOM), float(ZOOM) );

	// load / create new..

	// game mode
	// auto load persistent data
	// CTRL+S save persist data ( progress )


	// dev mode
	// left click on scene = toggle collision

	// CTRL+S to save dialogue and scene

	// load scene according to persist data


	std::unique_ptr<Dialogue> dialogue = std::make_unique<Dialogue>(asset, persist);
	std::unique_ptr<Scene> scene = std::make_unique<Scene>(asset, persist);


	renderscenesprite.setTexture( scene->renderscene.getTexture());

	dialogue->load_from_file(persist.get_current_dialogue_path());
	scene->load_from_file(persist.get_current_scene_path());


	// dialogue test sprite

	uint edit_collision_tiles = 2;
	bool edit_mode = false;

	uint time = 0;
	while(terminate == false)
	{
		while(window.pollEvent(event) == true)
		{
			if( event.type == sf::Event::Closed )
			{
				terminate = true;
			}



			if( event.type == sf::Event::MouseMoved)
			{
				sf::Vector2f pos = { float(event.mouseMove.x) / ZOOM, float(event.mouseMove.y) / ZOOM};
				if ( dialogue->is_active() )
				{
					if (!edit_mode)
					{
						// highlight dialogue option
						pointer_type = CURSOR_DEFAULT;

						dialogue->mouse_selected(pos);
					}
				}
				else
				{
					// highlight if current cursor
					pointer_type = scene->frob_has_sprite( pos ) ? CURSOR_USEABLE : CURSOR_DEFAULT;

					if (edit_mode && edit_collision_tiles < 2)
					{
						scene->set_grid_location(pos, edit_collision_tiles );
					}
				}

				pointer_sprite.setPosition( pos );

				switch (pointer_type)
				{
				case CURSOR_USEABLE:
					pointer_sprite.setOrigin(-1,-1);
					pointer_sprite.setTextureRect(sf::IntRect(8,0,8,8));
					break;

				case CURSOR_SPEECH:
					pointer_sprite.setOrigin(4,8);
					pointer_sprite.setTextureRect(sf::IntRect(0,8,8,8));
					break;

				default:
					pointer_sprite.setOrigin(-1,-1);
					pointer_sprite.setTextureRect(sf::IntRect(0,0,8,8));
				}
			}

			if (edit_mode)
			{
				if ( dialogue->is_active() )
				{
					// dev edit speech text:

					if ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down)
						dialogue->edit_down();

					if ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up)
						dialogue->edit_up();

					if ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left)
						dialogue->edit_left();

					if ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right)
						dialogue->edit_right();

					if (event.type == sf::Event::TextEntered)
					{
						if (event.text.unicode > 31 && event.text.unicode < 128)
						{
							dialogue->edit_insert((char)event.text.unicode);
						}
						else
						{
							if(event.text.unicode == 8) // backspace
							{
								dialogue->edit_backspace();
							}
							if(event.text.unicode == 13) // CR
							{
								dialogue->edit_newline();
							}
						}
					}
				}
				else
				{
					// keyboard to move view while editing collision tiles
					sf::Vector2f view_move;

					if ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down)
						view_move.y += 8.0f;

					if ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up)
						view_move.y -= 8.0f;

					if ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left)
						view_move.x -= 8.0f;

					if ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right)
						view_move.x += 8.0f;

					scene->view.move( view_move );
				}
			}

			if (edit_mode)
			{
				if( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S && event.key.control == true)
				{

					dialogue->save_to_file(persist.get_current_dialogue_path());
					scene->save_to_file(persist.get_current_scene_path());
				}
				// save persist data?

			}


			//else // PLAY mode

			if( event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left )
			{
				sf::Vector2f pos = { float(event.mouseButton.x) / ZOOM, float(event.mouseButton.y) / ZOOM};

				// only frob scene if theres no dialogue up
				if ( dialogue->is_active() )
				{
					dialogue->activate_highlighted_or_default();
				}
				else
				{
					if (!edit_mode)
					{
						SpeechID speech_id;
						if ( scene->frob( pos, speech_id ) )
						{
							scene->defocus();
							dialogue->activate( speech_id );
						}
					}



					//else
						//scene->toggle_grid_location(pos);
				}
			}

			if( event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right )
			{
				sf::Vector2f pos = { float(event.mouseButton.x) / ZOOM, float(event.mouseButton.y) / ZOOM};

				// quit dialogue
				if ( dialogue->is_active() )
				{
					dialogue->deactivate();
					scene->focus();
				}
			}

			if ( edit_mode && !dialogue->is_active() && event.type == sf::Event::MouseButtonPressed )
			{
				sf::Vector2f pos = { float(event.mouseButton.x) / ZOOM, float(event.mouseButton.y) / ZOOM};

				if ( event.mouseButton.button == sf::Mouse::Left )
					edit_collision_tiles = 0;
				else
					edit_collision_tiles = 1;
			}

			if (event.type == sf::Event::MouseButtonReleased )
				edit_collision_tiles = 2;

			if ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape )
			{
				edit_mode = !edit_mode;

				dialogue->edit_mode(edit_mode);
				scene->edit_mode(edit_mode);
			}

		}

		// Update
		frame_time = frame_clock.restart();

		dialogue->update();

		scene->update(time);


		// start, restart game = reset persist

		// if loading new scene:
		if (persist.state_flags.find("exit_to_crimescene") != persist.state_flags.end() )
		{
			persist.state_flags.erase("exit_to_crimescene");

			persist.scene_prefix = "crimescene";

			// reload scene:
			dialogue.reset();
			scene.reset();

			dialogue = std::make_unique<Dialogue>(asset, persist);
			scene = std::make_unique<Scene>(asset, persist);

			renderscenesprite.setTexture( scene->renderscene.getTexture());
			dialogue->load_from_file(persist.get_current_dialogue_path());
			scene->load_from_file(persist.get_current_scene_path());
		}


		// transition from one scene to another = reset scene

		// Render


		scene->renderscene.clear();
		scene->renderscene.setView(scene->view);
		scene->renderscene.draw(*scene);
		scene->renderscene.display();


		renderui.clear(sf::Color::Transparent);
		if ( dialogue->is_active() ) renderui.draw( *dialogue );
		if ( edit_mode )	          renderui.draw( edit_mode_sprite );
		renderui.draw(pointer_sprite);
		renderui.display();


		window.clear();
		window.draw(renderscenesprite);
		window.draw(renderuisprite);
		window.display();

		++time;
	}

	return 0;
}
