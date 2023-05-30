#include "Writer.h"
#include <fstream>
#include <iostream>     // cout, endl
#include <vector>
#include <string>
#include <algorithm>    // copy
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp>
#include "Parser.h"
#include <map>
#include <sstream>
#include <utf8/utf8.h>

std::map<int, int> encoding_map;

void Writer::ReadCSV(std::string path) {

	using namespace std;
	using namespace boost;
	//Bien veiller à ce que l'encoding du csv soit en UTF 8 sinon c'est la galère :!!!!!!!!!zedem:fzmef:

	typedef tokenizer<escaped_list_separator<char>> Tokenizer;

	TLs.clear();
	std::ifstream file(path);
	std::string str, line;
	
	int row_cnt = 0;
	bool inside_quotes(false);
	size_t last_quote(0);

	while (std::getline(file, str)) {
		row_cnt++;
		
		last_quote = str.find_first_of('"');
		while (last_quote != std::string::npos)
		{
			if (last_quote > 0) {
				if (str.at(last_quote - 1) != '\\')

				{
					inside_quotes = !inside_quotes;
				}
			}
			else inside_quotes = !inside_quotes;

			last_quote = str.find_first_of('"', last_quote + 1);
		}


		line.append(str);

		if (inside_quotes)
		{
			line.append("\n");
			continue;
		}

		escaped_list_separator<char> sep("\\", ";", "\"");

		Tokenizer tok(line, sep);

		
		int start = 0;
		auto it = tok.begin();

		std::string addr_str = (*it++);
		std::string orig = (*it++);
		std::string tl = (*it++);
		std::string comment = (*it++);
		uint32_t addr = -1;// std::atoi(addr_str.c_str());
		std::cout << tl << std::endl;
		Translation TL(addr, orig, tl);

		TLs.push_back(TL);

		line.clear();


	}



}


std::vector<uint8_t> intToBytes(unsigned int paramInt)
{	
	std::vector<uint8_t> res(4);
	for (int i = 0; i < 4; i++) res[i] = ((paramInt & (0xFF << (8 * i))) >> 8 * (i));
	return res;
}

void Writer::ReplaceIntAtIndex(uint32_t addr, uint32_t i) {


	auto dest_b = intToBytes(i);
	for (int idx = 0; idx < 4; idx++)
		this->content[addr +idx] = dest_b[idx];



}
void Writer::ReplaceShortAtIndex(uint32_t addr, uint16_t s) {

	std::vector<uint8_t> res(2);
	for (int i = 0; i < 2; i++) res[i] = ((s & (0xFF << (8 * i))) >> 8 * (i));

	for (int idx = 0; idx < 2; idx++)
		this->content[addr + idx] = res[idx];


}
void Writer::encrypt()
{
	uint32_t key = 0x14C823;
	uint32_t count = content.size();
	std::vector<uint8_t> k_b = intToBytes(key);
	std::vector<uint8_t> cnt_b = intToBytes(count);

	std::vector<uint8_t> out = {};
	out.insert(out.begin(), k_b.begin(), k_b.end());
	out.insert(out.end(), cnt_b.begin(), cnt_b.end());

	unsigned long k = (unsigned long)key;
	for (int i = 0; i < content.size(); i++)
	{
		key = key * 0x3d09;
		uint8_t added = content[i] + (uint8_t)(key >> 0x10);

		out.push_back(added);
	}
	this->content = out;
}

size_t CountTextBytes(const std::vector<uint8_t>& content, uint32_t addr) {

	size_t cnt = 0;
	while (content[addr+ cnt] != 0) {
	
		cnt++;
	}
	return cnt + 1;
}


//Took the functions from my text insertion tool for Ys I as is, didn't look further

std::vector<unsigned char> codeToByteArray(int k, int nb_bytes) {
	std::vector<unsigned char> res(nb_bytes);
	for (int i = 0; i < nb_bytes; i++) res[nb_bytes - i - 1] = ((k & (0xFF << (8 * i))) >> 8 * (i));

	return res;
}
int vectorToInt(std::vector<unsigned char> nb) {
	int result = 0;
	int cnt = 0;
	for (std::vector<unsigned char>::const_iterator i = nb.begin(); i != nb.end(); ++i) {
		result = result + (((*i) & 0x000000ff) << (cnt * 8));
		cnt++;
	}
	return result;
}

uint32_t ReadPtr(std::vector<uint8_t> &content, uint32_t addr) {
	int result = 0;
	int cnt = 0;
	for (int i = 0; i < 4; i++) {
		result = result + (((content[addr + i]) & 0x000000ff) << (i * 8));
	}
	return result;
}
uint16_t ReadU16(std::vector<uint8_t>& content, uint32_t addr) {
	int result = 0;
	int cnt = 0;
	for (int i = 0; i < 2; i++) {
		result = result + (((content[addr + i]) & 0x000000ff) << (i * 8));
	}
	return result;
}

void build_character_encoding() {
	std::cout << "Building character encoding from text.ini file..." << std::endl;
	std::ifstream file("text.ini");
	std::string str;
	while (getline(file, str))
	{
		int sjis, utf8;
		sscanf_s(str.c_str(), "%d %d", &utf8, &sjis);

		if (utf8 >= 0x80) encoding_map[utf8] = sjis;
	}
	file.close();
	std::cout << "DONE!" << std::endl;
}
std::vector<uint8_t> EncodeStr(std::string text) {
	std::vector<unsigned char> bytes;
	char next_chr;
	for (int idx_letter = 0; idx_letter < text.length(); idx_letter++) {
		char chr = text.at(idx_letter);
		if (idx_letter + 1 < text.length()) next_chr = text.at(idx_letter + 1);
		if (chr == '\\') {
			if (next_chr == 'x') {
				std::string hex_string = text.substr(idx_letter + 2, 2);

				int actual_byte;
				std::stringstream ss;
				ss << std::hex << hex_string;
				ss >> actual_byte;

				bytes.push_back((unsigned char)actual_byte);
				idx_letter += 3;
			}
		}
		else {
			std::vector<unsigned char> bytes_to_reencode;
			std::vector<unsigned char> new_bytes;
			int nb_bytes;

			if ((((unsigned char)chr) >= 0x80)) {
				nb_bytes = 2;
				bytes_to_reencode.push_back((unsigned char)chr);
				bytes_to_reencode.push_back((unsigned char)next_chr);
				idx_letter += 1;
				std::vector<unsigned int> utf32line;
				utf8::utf8to32(bytes_to_reencode.begin(), bytes_to_reencode.end(), back_inserter(utf32line));
				int correspondingSJIS = encoding_map[utf32line[0]];
				new_bytes = codeToByteArray(correspondingSJIS, nb_bytes);
			}
			else {
				nb_bytes = 1;
				new_bytes.push_back((unsigned char)chr);
			}
			bytes.insert(bytes.end(), new_bytes.begin(), new_bytes.end());
		}


	}
	bytes.push_back(0);
	return bytes;
}



void Writer::InsertTL(std::string path_original_file) {
	build_character_encoding();
	Parser p(path_original_file);
	p.decrypt();
	p.extract_TL();

	this->content = p.content;

	unsigned int idx_current_ptr = 0;
	unsigned int idx_current_jmp = 0;
	unsigned int text_idx = 0;
	uint32_t global_offset = 0;

	for (auto txt_addr : p.text_addrs) {
		for (int i = idx_current_ptr; i < p.pointeurs.size(); i++) {
			pointer ptr = p.pointeurs[i];
			if ((ptr.dest + ptr.offset) > txt_addr)
				break;
			idx_current_ptr++;
			ReplaceIntAtIndex(ptr.loc, (ptr.dest + ptr.offset) + global_offset); //we're storing the absolute value of the pointer here, we will deal with it later
		}
		for (int i = idx_current_jmp; i < p.jumps.size(); i++) {
			
			if (p.jumps[i].addr > txt_addr)
				break;
			idx_current_jmp++;
			p.jumps[i].addr = p.jumps[i].addr + global_offset;
		}
		size_t sz = CountTextBytes(this->content, txt_addr);
		std::vector<uint8_t> orig_text = std::vector<uint8_t>(this->content.begin() + txt_addr, this->content.begin() + txt_addr + sz);

		std::vector<uint8_t> new_text_bytes = EncodeStr(TLs[text_idx].translation);

		if (new_text_bytes.size() == 1)
			new_text_bytes = orig_text;
		global_offset = global_offset + new_text_bytes.size() - sz;
		text_idx = text_idx + 1;
	}

	

	global_offset = 0;
	text_idx = 0;
	// then we insert the new bytes
	for (auto txt_addr : p.text_addrs) {
		/*if (txt_addr == 0x14958)
			int a = 2;*/
		txt_addr = txt_addr + global_offset;



		size_t sz = CountTextBytes(this->content, txt_addr);
		std::vector<uint8_t> orig_text = std::vector<uint8_t>(this->content.begin() + txt_addr, this->content.begin() + txt_addr + sz);
		std::vector<uint8_t> new_text_bytes = EncodeStr(TLs[text_idx].translation);
		this->content.erase(this->content.begin() + txt_addr, this->content.begin() + txt_addr + sz);
		
		if (new_text_bytes.size() == 1)
			new_text_bytes = orig_text;
		this->content.insert(this->content.begin() + txt_addr, new_text_bytes.begin(), new_text_bytes.end());
		global_offset = global_offset + new_text_bytes.size() - sz;
		text_idx++;
	}

	//deal with the pointers at the start of each sections
	for (unsigned int i = 0; i <= 5; i++) {

		uint32_t sec_start = ReadPtr(this->content, 0xC + 0x4 * (i)) + 8;
		size_t count_sect_i = ReadU16(this->content, sec_start - 6);
		uint32_t end = sec_start + 4 * count_sect_i;

		for (unsigned int j = 0; j < count_sect_i; j++) {
			uint32_t absolute = ReadPtr(this->content, sec_start + 0x4 * j);
			ReplaceIntAtIndex(sec_start + 0x4 * j, absolute - end);
		}
	}

	//deal with the jumps within the script code
	std::sort(p.jumps.begin(), p.jumps.end(), compare_id);
	for (unsigned int jmp_idx = 0; jmp_idx < p.jumps.size() / 2; jmp_idx++) {
		jump jmpA = p.jumps[2 * jmp_idx];
		jump jmpB = p.jumps[2 * jmp_idx + 1];
		
		if (jmpA.dest) {
			uint16_t old = ReadU16(this->content, jmpB.addr);
			if (old != jmpA.addr - jmpB.addr)
				int a = 2;
			ReplaceShortAtIndex(jmpB.addr, jmpA.addr - jmpB.addr);
		}
		else {
			uint16_t old = ReadU16(this->content, jmpA.addr);
			if (old != jmpB.addr - jmpA.addr)
				int a = 2;
			ReplaceShortAtIndex(jmpA.addr, jmpB.addr - jmpA.addr);
		}

	}


	//Finally we fix the string length in the last section (not sure why it is even needed lol?)

	uint32_t addr_sect_5_start = ReadPtr(this->content, 0xC + 0x4 * 5) + 8;
	size_t count_sect_5 = ReadU16(this->content, addr_sect_5_start - 6);
	uint32_t addr_sect_5_ptr_end = addr_sect_5_start + 4 * count_sect_5;
	uint32_t addr_sect_5_end = content.size();

	uint32_t current_ptr = addr_sect_5_start;
	uint32_t current_data = 0;
	for (unsigned int i = 0; i < count_sect_5; i++) {

		current_data = ReadPtr(this->content, current_ptr) + addr_sect_5_ptr_end;
		current_ptr += 4;
		uint32_t raw_bytes = ReadPtr(this->content, current_data);
		size_t bytes_count_str = raw_bytes & 0xFF;
		uint16_t type = (raw_bytes & 0xFFFF0000) >> 16;
		uint32_t offset = 0;
		if ((type & 0x4000) == 0) {
		}
		else {
			offset = offset + 2;
		}
		if ((type & 0x8000) == 0) {

		}
		else {
			offset = offset + 2;
		}
		if ((type & 0x2000) == 0) {
		}
		else {
			offset = offset + 2;
		}
		if ((type & 0x1000) != 0) {
			offset = offset + 4;
		}
		size_t sz = CountTextBytes(this->content, current_data + offset + 4);
		this->content[current_data] = (uint8_t) sz;
	}
}

void Writer::WriteBinaryFile() {

	std::ofstream fout("ys2_libre.ys2_TLed", std::ios::out | std::ios::binary);
	fout.write((const char*)this->content.data(), this->content.size() * sizeof(uint8_t));
	fout.close();
}