#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <cstdint>

#include "json.hpp"
#include "load_json.hpp"

#include "asset.hpp"
#include "persist.hpp"
#include "dialogue.hpp"
#include "scene.hpp"

const sf::VideoMode VIDEO_MODE(1280, 720);

int main()
{
	sf::RenderWindow window(VIDEO_MODE, "sfmlgamejam05", sf::Style::Close);
	window.setFramerateLimit(120);

	sf::Event event;
	bool terminate = false;
	sf::Clock frame_clock;
	sf::Time frame_time = sf::Time::Zero;

	Asset asset;
	Dialogue dialogue(asset);
	Persist persist;
	std::unique_ptr<Scene> scene;

	// assets. load files here

	asset.load_texture("background", "data/tex/background.png");

	// load / create new..
	// dialogue data
	// persist data ( progress )

	// scene data

	// dev mode
	// CTRL+S to save each

	// load scene (start menu scene)

	scene = std::make_unique<Scene>(asset, dialogue, persist);

	while(terminate == false)
	{
		while(window.pollEvent(event) == true)
		{
			if( event.type == sf::Event::Closed )
			{
				terminate = true;
			}
			// mouse click and movement only
		}

		// Update
		frame_time = frame_clock.restart();

		// overlay of dialogue system = passed to scene

		// start/restart game = reset persist

		// transition from one scene to another = reset scene



		// Render
		window.clear();
		window.display();
	}

	return 0;
}
