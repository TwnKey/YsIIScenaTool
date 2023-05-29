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
uint16_t read_u16(const std::vector<uint8_t>& content, uint32_t& addr) {
	uint16_t out;
	memcpy(&out, content.data() + addr, sizeof(uint16_t));
	addr += sizeof(uint16_t);
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
			uint16_t wd;
			std::string text;
			std::cout << "new function " << std::endl;
			while (op_code != 1) {
				std::cout << std::hex << ptr << " OPCODE: " << (int)op_code << std::endl;
				ptr++;
				switch (op_code) {
					case 0x02:
					case 0x03:
					case 0x04:
					case 0x05:
					case 0x06:
					case 0x07:
					case 0x08:
					case 0x0A:
					case 0x15:
					case 0x17:
					case 0x18:
					case 0x21:
					case 0x3E:
					case 0x62:
					case 0x63:
					case 0x64:
					case 0x67:
					case 0x6C:
					case 0x79:
					case 0x7A:
					case 0x83:
					case 0xA0:
					case 0xA9:
					case 0xD2:
					case 0xD3:
					case 0xD5:
					case 0xD7:
						ptr += 2;
						break;
					case 0x09:
					case 0x0C:
					case 0x0D:
					case 0x0E:
					case 0x10:
					case 0x14:
					case 0x1A:
					case 0x1E:
					case 0x16:
					case 0x19:
					case 0x1C:
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
					case 0x56:
					case 0x5E:
					case 0x5F:
					case 0x6B:
					case 0x73:
					case 0x74:
					case 0xC7:
					case 0xC8:
					case 0xD1:
						break;
					case 0x38:
					case 0x39:
					case 0x4C:
					case 0x4D:
					case 0x8F:
					case 0xA3:
					case 0xB0:
					case 0xB6:
					case 0x9E:
					case 0x9F:
					case 0xAC:
					case 0xB1:
					case 0xB5:
					case 0xB8:
					case 0xBD:
					case 0xBE:
					case 0xD6:
						ptr += 3;
						break;
					case 0xB2:
						ptr += 13;
						break;
					case 0x3A:
					case 0x3D:
					case 0x58:
					case 0xB7:
					case 0xC1:
						ptr += 1;
						break;
					case 0xBB:
						ptr += 5;
						break;
					case 0x51:
						text = read_str(decrypted_content, ptr);
						output_csv << std::hex << ptr << ";\"" << text << "\";\"\";\"\";" << std::endl;
						break;
					case 0x11:
					case 0x13:
					case 0x55:
					case 0x60:
					case 0x61:
					case 0x6F:
					case 0xB3:
					case 0xB4:
					case 0xD0:
						cntt = decrypted_content[ptr++];
						for (unsigned int i = 0; i < cntt; i++)
							ptr+=2;
						break;
					case 0x8E:
					case 0x5D:
					
						cntt = decrypted_content[ptr++]-1;
						ptr += 2;
						for (unsigned int i = 0; i < cntt; i++)
							ptr += 2;
						break;
					case 0x0F:
					case 0x12:
					case 0x34:
					case 0x3F:
					case 0x57:
					case 0x5B:
					case 0x5C:
					case 0x69:
					case 0x6A:
					case 0x70:
					case 0x71:
					case 0x75:
					case 0x77:
					case 0x78:
					case 0x7B:
					case 0x7C:
					case 0x7E:
					case 0x7F:
					case 0x84:
					case 0x87:
					case 0x88:
					case 0x8A:
					case 0x8B:
					case 0x8C:
					case 0x8D:
					case 0x91:
					case 0x92:
					case 0x94:
					
					case 0xA2:
					case 0xA6:
					case 0xC9:
					case 0xCA:
					case 0xD9:
						ptr += 4;
						break;
					case 0x98:
					case 0x99:
					case 0xB9:
						ptr += 5;
						break;
					case 0x22:
					case 0x31:
					case 0x50:
					case 0x35:
					case 0x68:
					case 0x72:
					case 0x90:
					case 0x93:
					case 0x97:
					case 0x9B:
					case 0xA4:
					case 0xDA:
						ptr += 6;
						break;
					case 0x95:
					case 0xAB:
					case 0xC2:
					case 0xCB:
					case 0xCC:
						ptr += 8;
						break;
					case 0x25:
					case 0x26:
					case 0x27:
					case 0x53:
					case 0x80:
						ptr += 10;
						break;
					case 0x81:
					case 0xAA:
						ptr += 12;
						break;
					case 0x9D:
						ptr += 14;
						break;
					case 0x59:
						ptr += 16;
						break;
					case 0xCD:
						ptr += 18;
						break;
					case 0xCE:
						ptr += 20;
						break;
					case 0xC3:
						ptr += 28;
						break;
					case 0x7D:
						cntt = (decrypted_content[ptr++]- 3 >> 1) + 1;
						ptr += 4;
						for (unsigned int i = 0; i < cntt; i++)
							ptr += 4;
						break;
					case 0x9A:
						ptr += 3;
						text = read_str(decrypted_content, ptr);
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
					case 0x76:
						cntt = decrypted_content[ptr++];
						for (unsigned int i = 0; i < cntt;) {
							wd = read_u16(decrypted_content, ptr);
							if (wd == 0) {
								ptr += 4;
								i += 3;
							}
							else {
								i += 2;
								ptr += 2;

								if (wd == 3) {
									ptr += 4;
									i += 4;
								}
								else if (wd == 2) {
									ptr += 4;
									i += 4;
								}
								else {
								
								}
							
							}
						}
							
						break;
					default: 
						if (op_code > 220)
						{
						}
						else{
							std::cout << std::hex << ptr << " dec " << std::dec << (int)op_code << " hex " << std::hex << (int)op_code << std::endl;
							std::sort(op_codes.begin(), op_codes.end());
							op_codes.erase(std::unique(op_codes.begin(), op_codes.end()), op_codes.end());
							float prog = 100* (float)op_codes.size() / 220;
							std::cout << op_codes.size() << " " << prog << "%" << std::endl;
 							throw(std::exception("unknown op code"));
						}
					{}
				}
				
				op_codes.push_back(op_code);
				op_code = decrypted_content[ptr];
			}
			id++;
			
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
