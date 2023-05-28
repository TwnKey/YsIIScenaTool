// YsIIScenaTool.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

uint32_t read_u32(const std::vector<uint8_t>& content, uint32_t &addr) {
	uint32_t out;
	memcpy(&out, content.data() + addr, sizeof(uint32_t));
	addr += sizeof(uint32_t);
	return out;


}

std::string read_str(const std::vector<uint8_t>& content, uint32_t &addr) {
	std::vector<uint8_t> temp = {};

	while (content[addr] != 0) {
		temp.push_back(content[addr]);
		addr++;
	}
	std::string out(temp.begin(), temp.end());
	addr++;
	return out;
}

uint32_t read_u32_at(const std::vector<uint8_t>& content, uint32_t addr) {
	uint32_t out;
	memcpy(&out, content.data() + addr, sizeof(uint32_t));
	return out;


}
std::string read_str_at(const std::vector<uint8_t>& content, uint32_t addr) {
	std::vector<uint8_t> temp = {};
	
	while (content[addr] != 0) {
		temp.push_back(content[addr]);
		addr++;
	}
	std::string out(temp.begin(), temp.end());
	return out;


}
uint16_t read_u16_at(const std::vector<uint8_t>& content, uint32_t addr) {
	uint16_t out;
	memcpy(&out, content.data() + addr, sizeof(uint16_t));
	return out;


}
//Copied from Darkmet's text decryption
std::vector<uint8_t> decrypt(int key, std::vector<uint8_t>& content)
{
	std::vector<uint8_t> out = {};

	unsigned long k = (unsigned long)key;
	for (int i = 8; i < content.size(); i++)
	{
		key = key * 0x3d09;
		uint8_t op = (uint8_t)(key >> 0x10);
		out.push_back((uint8_t)(content[i] - op));
	}
	
	return out;
}



int main(int argc, char* argv[])
{
	if (argc == 2) {
		std::string path = argv[1];
		std::cout << "opening " << path << std::endl;
		std::ifstream file(path, std::ios::binary);
		std::vector<uint8_t> content = std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());
		
		uint32_t addr = 0;
		uint32_t key = read_u32(content, addr);
		std::cout << std::hex << key << std::endl;
		uint32_t count = read_u32(content, addr);
		std::cout << std::hex << content.size() << std::endl;
		std::vector<uint8_t> decrypted_content = decrypt(key, content);
		std::ofstream outfile("scena_decrypted", std::ios::out | std::ios::binary);
		outfile.write((const char *)(decrypted_content.data()), decrypted_content.size());

		uint32_t addr_sect_2_start = read_u32_at(decrypted_content, 0xC+ 0x4 * 1) + 8;
		size_t count_sect_2 = read_u16_at(decrypted_content, addr_sect_2_start - 6);
		uint32_t addr_sect_2_ptr_end = addr_sect_2_start + 4 * count_sect_2;
		uint32_t addr_sect_2_end = read_u32_at(decrypted_content, 0xC + 0x4 * 2) + 8;
		std::cout << std::hex << addr_sect_2_start << " " << addr_sect_2_ptr_end << " " << std::endl;

		uint32_t addr_sect_5_start = read_u32_at(decrypted_content, 0xC + 0x4 * 5) + 8;
		size_t count_sect_5 = read_u16_at(decrypted_content, addr_sect_5_start - 6);
		uint32_t addr_sect_5_ptr_end = addr_sect_5_start + 4 * count_sect_5;
		uint32_t addr_sect_5_end = decrypted_content.size();


		
		std::ofstream output_csv("to_translate.csv");        
		output_csv << "Address;Original;Translation;Comments;" << std::endl;          //Headings for file
		
		uint32_t current_addr = addr_sect_2_start;
		size_t id = 0; //tableau de fonctions à 0x4fad30, lecture de l'op code à 4636FE
		std::vector<uint8_t> op_codes = {};
		while (current_addr < addr_sect_2_ptr_end) {
			uint32_t ptr = read_u32(decrypted_content, current_addr) + addr_sect_2_ptr_end;
			uint8_t op_code = decrypted_content[ptr];
			uint8_t cntt; 
			std::string text;
			
			while (op_code != 1) {
				std::cout << std::hex << ptr << " " << (int)op_code << std::endl;
				ptr++;
				switch (op_code) {
					case 0x02:
					case 0x03:
					case 0x04:
					case 0x05:
					case 0x06:
					case 0x07:
					case 0x0A:
					case 0x17:
					case 0x18:
					case 0x3E:
					case 0x62:
					case 0x67:
					case 0x83:
					case 0xD5:
					case 0xD7:
						ptr += 2;
						break;
					case 0x0C:
					case 0x0D:
					case 0x0E:
					case 0x10:
					case 0x14:
					case 0x1E:
					case 0x37:
					
					case 0x4F:
					case 0x40:
					case 0x41:
					case 0x42:
					case 0x43:
					case 0x44:
					case 0x45:
					case 0x46:
					case 0x47:
					case 0x48:
					case 0x49:
					case 0x4A:
					case 0x4B:
					case 0x5F:
					case 0x73:
					case 0x74:
					case 0xD1:
						break;
					case 0x38:
					case 0x39:
					case 0x4C:
					case 0x4D:
					case 0xB6:
					case 0x9F:
					case 0xB1:
					case 0xB8:
					case 0xBE:
						ptr += 3;
						break;
					case 0xB2:
						ptr += 13;
						break;
					case 0x3A:
					case 0x3D:
					case 0xB7:
						ptr += 1;
						break;
					case 0xBB:
						ptr += 5;
						break;
					case 0x51:
						text = read_str(decrypted_content, ptr);
						output_csv << std::hex << ptr << ";\"" << text << "\";\"\";\"\";" << std::endl;
						break;
					
					case 0x6F:
						cntt = decrypted_content[ptr++];
						for (unsigned int i = 0; i < cntt; i++)
							ptr+=2;
						break;
					case 0x8E:
					case 0x5D:
						cntt = decrypted_content[ptr++];
						ptr += 2;
						for (unsigned int i = 0; i < cntt; i++)
							ptr += 2;
						break;
					
					case 0x0F:
					case 0x70:
					case 0x75:
					case 0x77:
					case 0x7C:
					case 0x87:
					case 0x88:
					case 0x8D:
					case 0x92:
					case 0x94:
						ptr += 4;
						break;
					case 0x22:
					case 0x72:
					case 0xA4:
						ptr += 6;
						break;
					case 0x95:
					case 0xC2:
						ptr += 8;
						break;
					case 0x27:
					case 0x53:
					case 0x80:
						ptr += 10;
						break;
					case 0x59:
						ptr += 16;
						break;
					case 0xCD:
						ptr += 18;
						break;
					case 0xDB:
						text = read_str(decrypted_content, ptr);
						output_csv << std::hex << ptr << ";\"" << text << "\";\"\";\"\";" << std::endl;
						break;
					case 0x52:
						ptr += 2;
						text = read_str(decrypted_content, ptr);
						output_csv << std::hex << ptr << ";\"" << text << "\";\"\";\"\";" << std::endl;
						ptr += 2;
						break;
					default: 
						std::cout << std::hex << ptr << " dec " << std::dec << (int)op_code << " hex " << std::hex << (int)op_code << std::endl;
						std::sort(op_codes.begin(), op_codes.end());
						op_codes.erase(std::unique(op_codes.begin(), op_codes.end()), op_codes.end());
						float prog = 100* (float)op_codes.size() / 220;
						std::cout << op_codes.size() << " " << prog << "%" << std::endl;
 						throw(std::exception("unknown op code"));
					{}
				}
				
				op_codes.push_back(op_code);
				op_code = decrypted_content[ptr];
			}
			
			
			/*if (id >= 0x01 && id <= (0x01 + 0x1D)) { //equipment
				std::string text = read_str_at(decrypted_content, ptr + 1);
				//std::cout << std::hex << ptr << " equipment " << id << " " << text  << std::endl;
			}

			if (id >= 0x1F && id <= (0x1F + 0x29)) { //items ; 0x1E is hardcoded in the exe
				std::string text = read_str_at(decrypted_content, ptr + 1);
				//std::cout << std::hex << ptr << " item " << id << " " << text << std::endl;
			}*/
			//std::cout << std::hex << ptr << " " << id << std::endl;
			id++;
			
		}
		for (auto op : op_codes) {
		
			std::cout << std::hex << (int) op << std::endl;
		
		}
		current_addr = addr_sect_5_start;
		while (current_addr < addr_sect_5_ptr_end) {
		
			uint32_t ptr = read_u32(decrypted_content, current_addr) + addr_sect_5_ptr_end;
			uint32_t raw_bytes = read_u32_at(decrypted_content, ptr);
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

			std::string text = read_str_at(decrypted_content, ptr + offset + 4);

			output_csv << std::hex << ptr << ";\"" << text << "\";\"\";\"\";" << std::endl;
		
		}
		output_csv.close();
	}
}
