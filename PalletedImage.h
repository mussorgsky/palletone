#include <vector>
#include "Chunk.h"

struct Color {
	int red, green, blue;
	Color(int r, int g, int b);
};

struct Range {
	int rate, flags, low, high;
	Range(int r, int f, int l, int h);
};

class PalletedImage {
public:
	int width, height;
	std::vector<Color> colors;
	std::vector<Range> ranges;
	std::vector<int> pixels;
	PalletedImage(std::vector<Chunk> chunks);
};