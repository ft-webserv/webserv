#include <ctime>
#include <string>

typedef struct s_request
{
	std::string _method;
	std::string _location;
	std::string _httpVersion;
	std::string _host;
	std::string _accept;
	std::string _connection;
	std::string _contentType;
	unsigned int _contentLength;
	std::string _body;
} t_request;

class Request
{
public:
	Request();
	~Request();
	void _readRequest(uintptr_t &clntSock, intptr_t data);

private:
	std::time_t startTime;
	std::string _requestBufs;
	t_request parsedRequest;
};
