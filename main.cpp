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

		PalletedImage image = PalletedImage(chunks);

		sf::RenderWindow window(sf::VideoMode(image.width, image.height), "Palletone");
		window.setFramerateLimit(15);
		
		sf::Texture texture;
		texture.create(image.width, image.height);
		sf::Uint8* pixels = new sf::Uint8[image.width * image.height * 4]; // 4 components (RGBA)
		
		for (int i = 0; i < image.pixels.size(); i++) {
			pixels[4 * i] = image.colors[image.pixels[i]].red;
			pixels[4 * i + 1] = image.colors[image.pixels[i]].green;
			pixels[4 * i + 2] = image.colors[image.pixels[i]].blue;
			pixels[4 * i + 3] = 255;
		}

		texture.update(pixels);

		sf::Sprite sprite;
		sprite.setTexture(texture);
		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
			}

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
