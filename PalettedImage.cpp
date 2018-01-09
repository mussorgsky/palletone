#include "PalettedImage.h"
#include <cmath>

Color::Color(int r, int g, int b)
{
	red = r;
	green = g;
	blue = b;
}

Range::Range(int r, int f, int l, int h)
{
	rate = r;
	flags = f;
	low = l;
	high = h;
}

PalettedImage::PalettedImage(std::vector<Chunk> chunks)
{
	int numPlanes, compression;
	width = 0;
	height = 0;
	quickestRate = 1;

	//policz ile BMHD
	int count = 0;
	int chi = 0;
	for (unsigned int i = 0; i < chunks.size(); i++)
	{
		if (chunks[i].chunkID == "BMHD")
		{
			chi = i;
			count++;
		}
	}
	(count != 1) ? std::cout << "Something wrong with BMHD\n" : std::cout << "BMHD GOOD\n";
	Chunk *ch = &chunks[chi];

	//zczytaj stuff z BMHD
	int idx = 0;
	int offset = idx;
	//width
	for (int mult = 256; idx < offset + 2; idx++)
	{
		width += ch->content[idx] * mult;
		mult /= 256;
	}

	offset = idx;
	//height
	for (int mult = 256; idx < offset + 2; idx++)
	{
		height += ch->content[idx] * mult;
		mult /= 256;
	}

	//pomin 4 bajty
	idx += 4;

	//numPlanes
	numPlanes = ch->content[idx++];

	//pomin 1 bajt
	idx += 1;

	//compression
	compression = ch->content[idx++];

	//napisz co tam
	std::cout << width << "x" << height << " " << numPlanes << " PLANES ";
	if (compression)
	{
		std::cout << "COMPRESSED";
	}
	std::cout << "\n";

	//policz ile CMAP
	count = 0;
	for (unsigned int i = 0; i < chunks.size(); i++)
	{
		if (chunks[i].chunkID == "CMAP")
		{
			chi = i;
			count++;
		}
	}
	(count != 1) ? std::cout << "Something wrong with CMAP\n" : std::cout << "CMAP GOOD\n";
	ch = &chunks[chi];

	//sprawdz czy CMAP->lenChunk / 3 = 2 ^ numPlanes
	(ch->lenChunk / 3 != pow(2, numPlanes)) ? std::cout << "Something wrong with numPlanes\n" : std::cout << "NUMPLANES GOOD\n";

	//stworz palete
	int r, g, b;
	for (int i = 0; i < ch->lenChunk; i += 3)
	{
		r = ch->content[i + 0];
		g = ch->content[i + 1];
		b = ch->content[i + 2];
		colors.push_back(Color(r, g, b));
	}
	std::cout << colors.size() << " color palette has been created\n";

	//zachowaj CRNGe
	for (unsigned int chi = 0; chi < chunks.size(); chi++)
	{
		if (chunks[chi].chunkID == "CRNG")
		{
			ch = &chunks[chi];
			int rate = 1;
			int flags = 0;
			int low, high;

			//pomin 2 bajty
			idx = 2;

			//rate
			offset = idx;
			for (int mult = 256; idx < offset + 2; idx++)
			{
				rate += ch->content[idx] * mult;
				mult /= 256;
			}
			std::cout << rate << ": ";
			rate *= (15.0 / 4096.0);
			std::cout << rate << "\n";

			//flags
			offset = idx;
			for (int mult = 256; idx < offset + 2; idx++)
			{
				flags += ch->content[idx] * mult;
				mult /= 256;
			}

			//low
			low = ch->content[idx++];

			//high
			high = ch->content[idx++];

			//pushuj range
			ranges.push_back(Range(rate, flags, low, high));

			if (rate > quickestRate)
			{
				quickestRate = rate;
			}
		}
	}
	std::cout << ranges.size() << " color ranges have been created\n";

	//policz ile BODY
	count = 0;
	for (unsigned int i = 0; i < chunks.size(); i++)
	{
		if (chunks[i].chunkID == "BODY")
		{
			chi = i;
			count++;
		}
	}
	(count != 1) ? std::cout << "Something wrong with BODY\n" : std::cout << "BODY GOOD\n";
	ch = &chunks[chi];

	//zczytaj piksele
	idx = 0;
	if (compression)
	{
		std::cout << ch->lenChunk << " compressed pixels found\n";
		std::vector<int> compressed;
		std::vector<int> decompressed;
		//czytaj kazdy bajt po kolei do compressed
		for (; idx < ch->lenChunk; idx++)
		{
			compressed.push_back(ch->content[idx]);
		}

		int next;
		int pos = 0;
		//dekompresja
		while (decompressed.size() < width * height)
		{
			int value = compressed[pos];
			next = pos + 1;
			if (value > 128)
			{
				for (int i = 0; i < 257 - value; i++)
				{
					decompressed.push_back(compressed[next]);
				}
				pos += 2;
			}
			else if (value < 128)
			{
				for (int i = 0; i < value + 1; i++)
				{
					decompressed.push_back(compressed[next + i]);
				}
				pos += compressed[pos] + 2;
			}
		}
		std::cout << decompressed.size() << " decompressed pixels have been created\n";

		//kopiuj decompressed w pixels
		pixels = decompressed;
	}
	else
	{
		//czytaj kazdy bajt po kolei do pixels
		for (; idx < ch->lenChunk; idx++)
		{
			pixels.push_back(ch->content[idx]);
		}
		std::cout << pixels.size() << " uncompressed pixels have been created\n";
	}
}

sf::Texture PalettedImage::makeTexture()
{
	sf::Texture texture;
	texture.create(width, height);
	sf::Uint8 *texturePixels = new sf::Uint8[width * height * 4]; // 4 components (RGBA)

	//to jest strasznie slaby sposob, trzeba cos z tym zrobic
	for (unsigned int i = 0; i < pixels.size(); i++)
	{
		texturePixels[4 * i] = colors[pixels[i]].red;
		texturePixels[4 * i + 1] = colors[pixels[i]].green;
		texturePixels[4 * i + 2] = colors[pixels[i]].blue;
		texturePixels[4 * i + 3] = 255;
	}

	texture.update(texturePixels);

	delete[] texturePixels;
	return texture;
}

sf::Texture PalettedImage::colorIndexTexture()
{
	sf::Texture texture;
	texture.create(width, height);
	sf::Uint8 *texturePixels = new sf::Uint8[width * height * 4]; // 4 components (RGBA)

	//to jest strasznie slaby sposob, trzeba cos z tym zrobic
	for (unsigned int i = 0; i < pixels.size(); i++)
	{
		texturePixels[4 * i] = pixels[i];
		texturePixels[4 * i + 1] = pixels[i];
		texturePixels[4 * i + 2] = pixels[i];
		texturePixels[4 * i + 3] = 255;
	}

	texture.update(texturePixels);

	delete[] texturePixels;
	return texture;
}

sf::Texture PalettedImage::paletteTexture()
{
	sf::Texture texture;
	texture.create(16, 16);
	sf::Uint8 *texturePixels = new sf::Uint8[16 * 16 * 4]; // 4 components (RGBA)

	//to jest strasznie slaby sposob, trzeba cos z tym zrobic
	for (unsigned int i = 0; i < colors.size(); i++)
	{
		texturePixels[4 * i] = colors[i].red;
		texturePixels[4 * i + 1] = colors[i].green;
		texturePixels[4 * i + 2] = colors[i].blue;
		texturePixels[4 * i + 3] = 255;
	}

	texture.update(texturePixels);

	delete[] texturePixels;
	return texture;
}

void PalettedImage::makeTexture(sf::Uint8 *texturePixels)
{
	for (unsigned int i = 0; i < pixels.size(); i++)
	{
		texturePixels[4 * i] = colors[pixels[i]].red;
		texturePixels[4 * i + 1] = colors[pixels[i]].green;
		texturePixels[4 * i + 2] = colors[pixels[i]].blue;
		texturePixels[4 * i + 3] = 255;
	}
}

void PalettedImage::cycleRanges(float dT)
{
	Range *r;
	for (unsigned int i = 0; i < ranges.size(); i++)
	{
		r = &ranges[i];
		//sprawdz czy cyklowac
		if (r->flags % 2 == 1)
		{
			//sprawdz czy cyklowac do tylu
			if (r->flags >> 1 == 1)
			{
				r->time += dT;
				if (r->time > 1.0f / (float)r->rate)
				{
					Color color = colors[ranges[i].low];
					colors.erase(colors.begin() + r->low);
					colors.insert(colors.begin() + r->high, color);
					r->time = 0;
				}
			}
			else
			{
				r->time += dT;
				if (r->time > 1.0f / (float)r->rate)
				{
					Color color = colors[ranges[i].high];
					colors.erase(colors.begin() + r->high);
					colors.insert(colors.begin() + r->low, color);
					r->time = 0;
				}
			}
		}
	}
}
