#include "Translation.h"
#include <fstream>

void WriteTLstoFile(std::vector<Translation> tls) {
	std::ofstream output_csv("to_translate.csv");
	output_csv << "Address;Original;Translation;Comments;" << std::endl;
	for (auto tl : tls) {
		output_csv << std::hex << tl.addr << ";\"" << tl.original << "\";\"" << tl.translation << "\";\"\";" << std::endl;
	}
	output_csv.close();
}