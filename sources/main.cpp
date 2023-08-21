#include "webserv.hpp"

int	main(int argc, char *argv[])
{
	if (argc == 2)
	{
		try
		{
			Config& configFile = Config::getConfFile();
			configFile.setFile(argv[1]);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
	else
		std::cout << "argument error!" << std::endl;
	return (0);
}
