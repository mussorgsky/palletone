#include <vector>
#include "Chunk.h"
#include <SFML/Graphics.hpp>

struct Color
{
	int red, green, blue;
	Color(int r, int g, int b);
};

struct Range
{
	int rate, flags, low, high;
	float time = 0;
	Range(int r, int f, int l, int h);
};

class PalettedImage
{
  public:
	unsigned int width, height;
	int quickestRate;
	std::vector<Color> colors;
	std::vector<Range> ranges;
	std::vector<int> pixels;
	PalettedImage(std::vector<Chunk> chunks);
	sf::Texture makeTexture();
	sf::Texture colorIndexTexture();
	sf::Texture paletteTexture();
	void makeTexture(sf::Uint8 *);
	void cycleRanges(float);
};
