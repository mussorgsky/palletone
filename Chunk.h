#pragma once
#include <string>
#include <vector>
#include <iostream>

class Chunk {
public:
	std::string chunkID;
	int lenChunk;
	std::vector<unsigned char> content;
	bool isPadded;

	Chunk();
	Chunk(std::string id, int length, std::vector<unsigned char> data);
	void printInfo();

	static std::vector<Chunk> Chunk::makeChunks(unsigned char * data);
};