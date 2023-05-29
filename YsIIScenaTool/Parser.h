#pragma once

#include <vector>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "Translation.h"
class Parser
{

public:

	unsigned int internal_addr = 0;
	std::vector<uint8_t> content = {};

	Parser(std::string path)
	{
		std::cout << "opening " << path << std::endl;
		std::ifstream file(path, std::ios::binary);
		std::vector<uint8_t> f = std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());
		this->content = f;
	
	};

	void decrypt();
	uint32_t read_u32();
	uint16_t read_u16();
	std::string read_str();
	uint32_t read_u32_at(uint32_t addr);
	std::string read_str_at(uint32_t addr);
	uint16_t read_u16_at(uint32_t addr);
	std::vector<Translation> extract_TL();


};