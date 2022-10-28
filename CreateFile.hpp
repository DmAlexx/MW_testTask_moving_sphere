#pragma once
#include <iostream>
#include <fstream>

class CreateFile
{
private:
	std::ofstream m_writeTheFile;
	std::string m_skinFileName;
public:
	CreateFile(const std::string& skinFileName);
	~CreateFile();
	void writeToFile(char buffer[64]);
};

