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
	window.setFramerateLimit(120);
	window.setMouseCursorVisible(false);


	sf::RenderTexture renderscene;
	renderscene.create(ZOOM_W, ZOOM_H, false);

	sf::View view =	renderscene.getDefaultView();
	view.setCenter( { int(ZOOM_W/2),int(ZOOM_H/2)} );

	sf::Sprite renderscenesprite;
	renderscenesprite.setTexture(renderscene.getTexture());
	renderscenesprite.setScale( float(ZOOM), float(ZOOM) );

	sf::RenderTexture renderui;
	renderui.create(ZOOM_W, ZOOM_H, false);

	sf::Sprite renderuisprite;
	renderuisprite.setTexture(renderui.getTexture());
	renderuisprite.setScale( float(ZOOM), float(ZOOM) );

	Asset asset;

	// load files here
//asset.load_font("pixel", "data/font/434.ttf");
	asset.load_font("pixel", "data/font/ARCADEPI.TTF");
	asset.load_texture("background", "data/tex/background.png");
	asset.load_texture("placeholder", "data/tex/placeholder.png");
	asset.load_texture("box", "data/tex/box.png");
	asset.load_texture("text_cursor", "data/tex/text_cursor.png");
	asset.load_texture("police_station", "data/tex/police_station.png");
	asset.load_texture("ui", "data/tex/ui.png");

	asset.load_shader("fader", "data/shaders/fader_v.glsl", "data/shaders/fader_f.glsl");

	sf::Sprite pointer_sprite( *asset.textures["ui"] );
	pointer_sprite.setTextureRect( sf::IntRect(0,0,8,8) );

	uint pointer_type = USE_DEFAULT;

	sf::Sprite edit_mode_sprite( *asset.textures["ui"] );
	edit_mode_sprite.setTextureRect( sf::IntRect(32,0,32,16) );

	edit_mode_sprite.setPosition( ZOOM_W-32, 0 );

	Persist persist;

	Dialogue dialogue(asset, persist);

	// load / create new..

	dialogue.load_from_file("data/dialogue.txt");

	//dialogue.sprite.set_font(*asset.fonts["pixel"]);

	dialogue.default_font = asset.fonts["pixel"].get();

	//dialogue.setPosition(32,32);




	// persist data ( progress )

	// scene data

	// dev mode
	// CTRL+S to save each

	// load scene (start menu scene)


	std::unique_ptr<Scene> scene = std::make_unique<Scene>(asset, dialogue, persist);

	uint edit_mode = MODE_CREATE;

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
				if ( dialogue.is_active() )
				{
					if (!edit_mode)
					{
						// highlight dialogue option
						pointer_type = USE_DEFAULT;

						dialogue.mouse_selected(pos);
					}
				}
				else
				{
					// highlight if current cursor option matches frob_option
					MegaSprite* sprite = scene->frob_has_sprite( pos );
					if (sprite != nullptr)
					{
						if ( sprite->use_types.size() )
						{
							bool has_current_type = false;
							for ( uint ut : sprite->use_types)
							{
								if (pointer_type == ut) has_current_type = true;
							}

							if (!has_current_type)
							{
								pointer_type = sprite->use_types.front();
							}
						}
					}
					else
					{
						pointer_type = USE_DEFAULT;
					}
				}

				pointer_sprite.setPosition( pos );

				switch (pointer_type)
				{
				case USE_SPEECH:
					pointer_sprite.setOrigin(4,8);
					pointer_sprite.setTextureRect(sf::IntRect(0,8,8,8));
					break;

				default:
					pointer_sprite.setOrigin(-1,-1);
					pointer_sprite.setTextureRect(sf::IntRect(0,0,8,8));
				}
			}

			// dev edit speech text:

			// mode to select the speech string to edit
			if (edit_mode)
			{
				if ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down)
					dialogue.edit_down();

				if ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up)
					dialogue.edit_up();

				if ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left)
					dialogue.edit_left();

				if ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right)
					dialogue.edit_right();

				if (event.type == sf::Event::TextEntered)
				{
					if (event.text.unicode > 31 && event.text.unicode < 128)
					{
						dialogue.edit_insert((char)event.text.unicode);
					}
					else
					{
						if(event.text.unicode == 8) // backspace
						{
							dialogue.edit_backspace();
						}
						if(event.text.unicode == 13) // CR
						{
							dialogue.edit_newline();
						}
					}
				}

				if( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S && event.key.control == true)
				{
					dialogue.save_to_file("data/dialogue.txt");
				}
			}

			//else // PLAY mode

			if( event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left )
			{
				sf::Vector2f pos = { float(event.mouseButton.x) / ZOOM, float(event.mouseButton.y) / ZOOM};
				std::cout << "pressed. " << pos.x << " " << pos.y << std::endl;

				// only frob scene if theres no dialogue up
				if ( dialogue.is_active() )
				{
					dialogue.activate_highlighted();
				}
				else
				{
					scene->frob( pos );
				}
			}

			if( event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right )
			{
				sf::Vector2f pos = { float(event.mouseButton.x) / ZOOM, float(event.mouseButton.y) / ZOOM};
				std::cout << "pressed. " << pos.x << " " << pos.y << std::endl;

				// quit dialogue
				if ( dialogue.is_active() )
				{
					dialogue.deactivate();
					scene->focus();
				}
			}

			if ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape )
			{
				edit_mode = !edit_mode;
				dialogue.edit_mode(edit_mode);
			}

		}

		// Update
		frame_time = frame_clock.restart();

		dialogue.update();
		scene->update();

		// start, restart game = reset persist

		// transition from one scene to another = reset scene

		// Render


		renderscene.clear();
		renderscene.setView(view);
		renderscene.draw(*scene);
		renderscene.display();


		renderui.clear(sf::Color::Transparent);
		if ( dialogue.is_active() ) renderui.draw( dialogue );
		if ( edit_mode )	     renderui.draw( edit_mode_sprite );
		renderui.draw(pointer_sprite);
		renderui.display();


		window.clear();
		window.draw(renderscenesprite);
		window.draw(renderuisprite);
		window.display();
	}

	return 0;
}
