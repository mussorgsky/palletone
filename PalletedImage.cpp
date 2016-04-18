#include "PalletedImage.h"

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

PalletedImage::PalletedImage(std::vector<Chunk> chunks)
{
	int numPlanes, compression;
	width = 0;
	height = 0;

	//policz ile BMHD
	int count = 0;
	int chi;
	for (int i = 0; i < chunks.size(); i++) {
		if (chunks[i].chunkID == "BMHD") {
			chi = i;
			count++;
		}
	}
	(count != 1) ? std::cout << "Something wrong with BMHD\n" : std::cout << "BMHD GOOD\n";
	Chunk * ch = &chunks[chi];

	//zczytaj stuff z BMHD
	int idx = 0;
	int offset = idx;
	//width
	for (int mult = 256; idx < offset + 2; idx++) {
		width += ch->content[idx] * mult;
		mult /= 256;
	}
	std::cout << width << " <--- width\n";

	offset = idx;
	//height
	for (int mult = 256; idx < offset + 2; idx++) {
		height += ch->content[idx] * mult;
		mult /= 256;
	}
	std::cout << height << " <--- height\n";

	//pomin 4 bajty
	idx += 4;

	//numPlanes
	numPlanes = ch->content[idx++];
	std::cout << numPlanes << " <--- numPlanes\n";

	//pomin 1 bajt
	idx += 1;

	//compression
	compression = ch->content[idx++];
	std::cout << compression << " <--- compression\n";

	//policz ile CMAP
	count = 0;
	for (int i = 0; i < chunks.size(); i++) {
		if (chunks[i].chunkID == "CMAP") {
			chi = i;
			count++;
		}
	}
	(count != 1) ? std::cout << "Something wrong with CMAP\n" : std::cout << "CMAP GOOD\n";
	ch = &chunks[chi];

	//sprawdz czy CMAP->lenChunk / 3 = 2 ^ numPlanes
	(ch->lenChunk / 3 != std::pow(2, numPlanes)) ? std::cout << "Something wrong with numPlanes\n" : std::cout << "NUMPLANES GOOD\n";

	//stworz palete
	int r, g, b;
	for (int i = 0; i < ch->lenChunk; i += 3) {
		r = ch->content[i + 0];
		g = ch->content[i + 1];
		b = ch->content[i + 2];
		colors.push_back(Color(r, g, b));
	}
	std::cout << colors.size() << " color palette has been created\n";

	//zachowaj CRNGe
	for (int chi = 0; chi < chunks.size(); chi++) {
		if (chunks[chi].chunkID == "CRNG") {
			ch = &chunks[chi];
			int rate = 0;
			int flags = 0;
			int low, high;
			
			//pomin 2 bajty
			idx = 2;

			//rate
			for (int mult = 256; idx < offset + 2; idx++) {
				rate += ch->content[idx] * mult;
				mult /= 256;
			}

			//flags
			for (int mult = 256; idx < offset + 2; idx++) {
				flags += ch->content[idx] * mult;
				mult /= 256;
			}

			//low
			low = ch->content[idx++];

			//high
			high = ch->content[idx++];

			//pushuj range
			ranges.push_back(Range(rate, flags, low, high));
		}
	}
	std::cout << ranges.size() << " color ranges have been created\n";

	//policz ile BODY
	count = 0;
	for (int i = 0; i < chunks.size(); i++) {
		if (chunks[i].chunkID == "BODY") {
			chi = i;
			count++;
		}
	}
	(count != 1) ? std::cout << "Something wrong with BODY\n" : std::cout << "BODY GOOD\n";
	ch = &chunks[chi];

	//zczytaj piksele
	idx = 0;
	if (compression == 0) {
		//czytaj kazdy bajt po kolei do pixels
		for (; idx < ch->lenChunk; idx++) {
			pixels.push_back(ch->content[idx]);
		}
		std::cout << pixels.size() << " uncompressed pixels have been created\n";
	}
	else {
		std::cout << ch->lenChunk << " compressed pixels found\n";
		std::vector<int> compressed;
		std::vector<int> decompressed;
		//czytaj kazdy bajt po kolei do compressed
		for (; idx < ch->lenChunk; idx++) {
			compressed.push_back(ch->content[idx]);
		}

		int pos = 0;
		int read = 0;
		//dekompresja
		while (decompressed.size() < width * height) {
			int value = compressed[pos];
			if (value > 128) {
				read = pos + 1;
				for (int i = 0; i < 257 - value; i++) {
					decompressed.push_back(compressed[read]);
				}
				pos += 2;
			}
			else if (value < 128) {
				for (int i = 0; i < value + 1; i++) {
					decompressed.push_back(compressed[pos + 1 + i]);
				}
				pos += compressed[pos] + 2;
			}
		}
		std::cout << decompressed.size() << " decompressed pixels have been created\n";
		
		//kopiuj decompressed w pixels
		pixels = decompressed;
	}
}
