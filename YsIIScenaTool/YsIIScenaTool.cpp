// YsIIScenaTool.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include "Parser.h"
int main(int argc, char* argv[])
{
	if (argc == 2) {
		std::string path = argv[1];
		
		Parser p(path);
		p.decrypt();
		std::vector<Translation> TLs = p.extract_TL();
		WriteTLstoFile(TLs);
	}
}
