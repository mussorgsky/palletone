#include <fstream>
#include <iostream>
#include <vector>
#include "Chunk.h"
#include "PalletedImage.h"
#include <SFML/Graphics.hpp>

int main(int argc, char * argv[])
{
	if (argc < 2) {
		std::cout << "No file specified, exiting.\n";
		return 0;
	}
	std::ifstream in(argv[1], std::ios::in | std::ios::binary | std::ios::ate);
	if (in.is_open()) {
		unsigned char * data;
		std::vector<Chunk> chunks;

		int size = in.tellg();
		data = new unsigned char[size];

		in.seekg(0, std::ios::beg);
		in.read((char *)data, size);
		in.close();

		chunks = Chunk::makeChunks(data);
		delete[] data;

		PalettedImage image = PalettedImage(chunks);

		sf::RenderWindow window(sf::VideoMode(image.width, image.height), "Palletone");
		window.setFramerateLimit(60);

		sf::Sprite sprite;
		sf::Texture texture = image.makeTexture();
		sprite.setTexture(texture);

		sf::Clock deltaClock;
		sf::Time deltaTime;

		while (window.isOpen())
		{
			deltaTime = deltaClock.restart();
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
			}

			image.cycleRanges(deltaTime.asSeconds());
			texture = image.makeTexture();
			sprite.setTexture(texture);

			window.clear();
			window.draw(sprite);
			window.display();
		}
	}
	else {
		std::cout << "Could not open\n";
	}

	return 0;
}
