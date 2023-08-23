#ifndef Server_HPP
#define Server_HPP

class Server
{
public:
	Server(void);
	Server(const Server &src);
	Server &operator=(Server const &rhs);
	virtual ~Server(void);

private:
	const int socketID;
};

#endif
