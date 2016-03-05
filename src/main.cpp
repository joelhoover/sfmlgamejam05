#include <SFML/Graphics.hpp>

#include <cstdint>

const sf::VideoMode VIDEO_MODE(1280, 720);

int main()
{
	sf::RenderWindow window(VIDEO_MODE, "sfmlgamejam05", sf::Style::Close);
	window.setFramerateLimit(120);

	sf::Event event;
	bool terminate = false;
	sf::Clock frame_clock;
	sf::Time frame_time = sf::Time::Zero;

	while(terminate == false) {
		while(window.pollEvent(event) == true) {
			if(event.type == sf::Event::KeyPressed) {
				if(event.key.code == sf::Keyboard::Escape) {
					terminate = true;
				}
			}
			// mouse click and movement only
		}

		// Update
		frame_time = frame_clock.restart();

		// Render
		window.clear();
		window.display();
	}

	return 0;
}
