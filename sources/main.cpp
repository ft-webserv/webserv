#include "Config.hpp"

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
		(void)argc;
		Config configTest(argv[1]);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
