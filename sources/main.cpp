#include "Config.hpp"
#include "ServerManager.hpp"

/*
		Our Webserv Flow

		START
		|
		Config Parse
		|
		Object Genrate
		|
		listen()
	__	|
	|	kevent Occured
loop|	|										____
	|__	Ident Check									|
		|					|						|
		(server)			(client)				|
		|					|						|
		connect() Client	Filter Check			|	loop
							|			|			|
							(read)		(write)		|
							|			|			|
							Response	Request	____|
*/

int main(int argc, char *argv[])
{
	try
	{
		if (argc == 1)
			Config::getInstance("./Config/shy.config");
		else if (argc == 2)
			Config::getInstance(argv[1]);
		else
			Exception::unvalidArgumentsError();

		ServerManager serverManager;
		serverManager.runServer();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
