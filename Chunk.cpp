#include "Chunk.h"

Chunk::Chunk()
{
}

Chunk::Chunk(std::string id, int length, std::vector<unsigned char> data)
{
	chunkID = id;
	lenChunk = length;
	content = data;
}

void Chunk::printInfo()
{
	char id[5];
	for (int i = 0; i < 4; i++) id[i] = chunkID[i];
	id[4] = 0;
	std::cout << "chunkID: " << id << "\n";
	std::cout << "lenChunk: " << lenChunk << "\n";
	std::cout << "isPadded: " << isPadded << "\n\n";
}

std::vector<Chunk> Chunk::makeChunks(unsigned char * data)
{
	std::vector<Chunk> chunks;
	int idx = 0;
	int offset = 0;
	//sprawdz czy pierwsze 4 bajty to "FORM"
	char form[5];
	for (; idx < 4; idx++) {
		form[idx - offset] = data[idx];
	}
	form[4] = 0;
	if ((std::string)form != "FORM") { std::cout << "Shit's broken yo, not FORM\n"; }

	offset = idx;
	int length = 0;
	//czytaj nastepne 4 bajty zapamietaj dlugosc
	for (int mult = 16777216; idx < offset + 4; idx++) {
		length += data[idx] * mult;
		mult /= 256;
	}
	std::cout << "Data length: " << length << "\n";

	offset = idx;
	//sprawdz czy nastepne 4 bajty to "PBM "
	for (; idx < offset + 4; idx++) {
		form[idx - offset] = data[idx];
	}
	if ((std::string)form != "PBM ") { std::cout << "Shit's broken yo, not PBM\n"; }

	//sprawdz czy nie koniec pliku
	while (idx < length) {
		std::string chunkID = "NULL";
		int lenChunk = 0;
		std::vector<unsigned char> content;
		
		offset = idx;
		//czytaj 4 bajty -> chunkID
		for (; idx < offset + 4; idx++) {
			chunkID[idx - offset] = data[idx];
		}
		
		offset = idx;
		//czytaj 4 bajty -> lenChunk
		for (int mult = 16777216; idx < offset + 4; idx++) {
			lenChunk += data[idx] * mult;
			mult /= 256;
		}

		offset = idx;
		//czytaj n bajtow, zapamietaj
		for (; idx < offset + lenChunk; idx++) {
			content.push_back(data[idx]);
		}

		//dodaj padding jesli lenChunk nieparzyste
		if (lenChunk % 2) { idx++; }

		//wypisz co sie dzieje w ogole
		std::cout << chunkID << " " << lenChunk << "\n";

		//zrob nowy chunk z tego syfu
		chunks.push_back(Chunk(chunkID, lenChunk, content));
	}
	return chunks;
}
