#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <exception>

typedef struct	s_fileInfo
{
	std::ifstream	file;
	std::string		buf;
	int				bufSize;
}				t_fileInfo;

class Config
{
private:
	static Config*						confFile;
	std::map<std::string, std::string>	confParse;
	Config();
	Config(const Config& source);
	~Config();

public:
	static Config*								getConfFile();
	void										openFile(char *fileName);
	void										setFile(char *fileName);
	std::map<std::string, std::string>			getFile();
	class FileError : public std::exception
	{
	public:
		virtual const char *what() const throw();
	};
};

Config* Config::confFile = NULL;

#endif
