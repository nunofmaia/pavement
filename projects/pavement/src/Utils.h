#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

class Utils{
public:
	inline static std::string readFile(std::string filePath){
		std::ifstream myFile;
		std::string aux,myStringFile;
		myFile.open(filePath, std::ifstream::in);

		if (myFile.is_open()) {
			while(getline(myFile,aux)) {
				myStringFile = myStringFile + aux + '\n';
			}

			return myStringFile;
		} else {
			return NULL;	
			std::cerr << "Could not read file: "<< filePath << std::endl;
		}
	}
};

#endif