#include <SFML/Graphics.hpp>

#include <cstdint>

#include "asset.hpp"
#include "scene.hpp"
#include "persist.hpp"
#include "dialogue.hpp"

#include "json.hpp"
#include "load_json.hpp"

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
	Persist persist;
	Dialogue dialogue;
	Scene scene;

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
