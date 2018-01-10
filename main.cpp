#include <fstream>
#include <iostream>
#include <vector>
#include "Chunk.h"
#include "PalettedImage.h"
#include <SFML/Graphics.hpp>

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cout << "No file specified, exiting.\n";
		return 0;
	}
	std::ifstream in(argv[1], std::ios::in | std::ios::binary | std::ios::ate);
	if (in.is_open())
	{
		unsigned char *data;
		std::vector<Chunk> chunks;

		int size = (int)in.tellg();
		data = new unsigned char[size];

		in.seekg(0, std::ios::beg);
		in.read((char *)data, size);
		in.close();

		chunks = Chunk::makeChunks(data);
		delete[] data;

		PalettedImage image = PalettedImage(chunks);
		chunks.clear();

		int i = 0;
		size_t idx = 0;
		while (argv[1][i + 1] != 0)
		{
			if (argv[1][i] == '\\' || argv[1][i] == '/')
			{
				idx = i + 1;
			}
			i++;
		}

		sf::String s = argv[1];
		s = s.substring(idx, sf::String::InvalidPos);

		sf::RenderWindow window(sf::VideoMode(image.width, image.height), s + " - Palletone ");
		window.setFramerateLimit(image.quickestRate);
		std::cout << "Frame limit: " << image.quickestRate << std::endl;

		sf::Texture cIndexes = image.colorIndexTexture();
		sf::Texture palette = image.paletteTexture();

		sf::Shader shader;
		const std::string shaderSource = R"(
#version 300 es
precision mediump float;

uniform vec2 Resolution;
uniform float ColCount;
uniform sampler2D Texture;
uniform sampler2D Palette;

out vec4 Color;

#define PI 3.1415

void main() {
    vec2 UV = gl_FragCoord.xy / Resolution;
    UV.y = (UV.y - 0.5) * -1. + 0.5;
    
    int i = int(texture2D(Texture, UV).r * ColCount);
    int h = int(sqrt(ColCount));

    vec2 pUV = vec2(float(i % h), float(i / h)) / vec2(h);
    
	Color = vec4(texture2D(Palette, pUV));
}
)";
		shader.loadFromMemory(shaderSource, sf::Shader::Fragment);
		shader.setUniform("Resolution", sf::Vector2f((float)image.width, (float)image.height));
		shader.setUniform("ColCount", (float)image.colors.size());
		shader.setUniform("Texture", cIndexes);
		shader.setUniform("Palette", palette);

		sf::RectangleShape shape(sf::Vector2f((float)image.width, (float)image.height));
		shape.setFillColor(sf::Color::Magenta);

		// sf::Sprite sprite;
		// sf::Texture texture = image.makeTexture();
		// sf::Texture texture = image.colorIndexTexture();
		// sf::Texture texture = image.paletteTexture();
		// sprite.setTexture(texture);

		// sf::Uint8 *texturePixels = new sf::Uint8[image.width * image.height * 4]; // 4 components (RGBA)

		sf::Clock deltaClock;
		sf::Time deltaTime;
		sf::Event event;

		int frames = 0;
		float time = 0;
		const int testPeriod = 5;

		while (window.isOpen())
		{
			deltaTime = deltaClock.restart();
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
			}

			image.cycleRanges(deltaTime.asSeconds());
			palette = image.paletteTexture();
			shader.setUniform("Palette", palette);
			// image.makeTexture(texturePixels);
			// texture.update(texturePixels);

			window.clear();
			// window.draw(sprite);
			window.draw(shape, &shader);
			window.display();

			frames++;
			time += deltaTime.asSeconds();

			if (time > testPeriod)
			{
				std::cout << time / frames * 1000.0 << "ms " << frames / time << "fps over last " << testPeriod << "s\n";
				time = 0;
				frames = 0;
			}
		}

		// delete[] texturePixels;
	}
	else
	{
		std::cout << "Could not open\n";
	}

	return 0;
}
