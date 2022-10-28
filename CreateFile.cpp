#include "CreateFile.hpp"

CreateFile::CreateFile(const std::string& skinFileName)
	:m_skinFileName(skinFileName)
{
	m_writeTheFile.open(m_skinFileName, std::ios_base::out);
	if (!m_writeTheFile.is_open())
	{
		std::cout << "Error open " << m_skinFileName.substr(m_skinFileName.find_last_of("") + 1, std::string::npos) << " file" << std::endl;
		perror("");
		std::cout << std::endl;
		m_writeTheFile.open(m_skinFileName.substr(3), std::ios_base::out);

		if (!m_writeTheFile.is_open())
		{
			std::cout << "Error open " << m_skinFileName.substr(m_skinFileName.find_last_of("\\") + 1, std::string::npos) << " file" << std::endl;
			perror("");
		}
		else
		{
			std::cout << "File " << m_skinFileName.substr(m_skinFileName.find_last_of("\\") + 1, std::string::npos) << " created in current directory" << std::endl;
		}
	}
}

CreateFile::~CreateFile()
{
	m_writeTheFile.close();
}

void CreateFile::writeToFile(char buffer[64])
{
	m_writeTheFile << buffer;
}