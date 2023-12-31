#include "Cgi.hpp"
#include "Client.hpp"

Cgi::Cgi(Request *request, Response *response, uintptr_t socket, Client *client)
{
	_request = request;
	_response = response;
	_clientSock = socket;
	_client = client;
	_envCnt = 0;
	_reqFd[0] = 0;
	_reqFd[1] = 0;
	_resFd[0] = 0;
	_resFd[1] = 0;
	_pid = 0;
	_env = new char *[ENVMAXSIZE];
	_requestBody = request->getParsedRequest()._body;
	_requestBodyLength = _requestBody.length();
	_lastPos = 0;
	_cgiInfo = response->getCgiInfo();
	_makeEnvList(_clientSock);
}

Cgi::~Cgi()
{
	for (int i = 0; i < _envCnt; i++)
		delete[] _env[i];
	delete[] _env;
}

uintptr_t Cgi::getClientSock() { return (_clientSock); }

Response *Cgi::getResponse() { return (_response); }

pid_t Cgi::getPid() { return (_pid); }

Client *Cgi::getClient() { return (_client); }

void Cgi::_makeEnvList(uintptr_t clntSock)
{
	struct sockaddr_in clnt;
	socklen_t clntSockLen = sizeof(clnt);
	port_t port;
	std::map<std::string, std::string>::iterator iter = _request->getEtcHeader().begin();
	std::map<std::string, std::string>::iterator end = _request->getEtcHeader().end();

	getsockname(clntSock, (sockaddr *)&clnt, &clntSockLen);
	port = ntohs(clnt.sin_port);
	for (; iter != end; iter++)
	{
		std::string str = iter->first;

		std::for_each(str.begin(), str.end(), ft_toupper);
		str.pop_back();
		if (str == "X_SECRET_HEADER_FOR_TEST")
			str = "HTTP_" + str;
		_addEnv(str, iter->second);
	}

	_addEnv("CONTENT_TYPE", _request->getParsedRequest()._contentType);
	_addEnv("CONTENT_LENGTH", ft_itos(_request->getParsedRequest()._body.size()));
	_addEnv("SERVER_NAME", mapFind(_response->getServerInfo()->getServerInfo(), "server_name"));
	_addEnv("SERVER_PROTOCOL", "HTTP/1.1");
	_addEnv("PATH_INFO", _request->getParsedRequest()._location);
	_addEnv("PATH_INFO", "/directory/youpi.bla");
	_addEnv("SERVER_SOFTWARE", "webserv/0.1");
	_addEnv("SERVER_PORT", ft_itos(port));
	_addEnv("GETWAY_INTERFACE", "CGI/1.1");
	_addEnv("REQUEST_METHOD", _request->getParsedRequest()._method);
	_addEnv("REMOTE_ADDR", ft_itos(clnt.sin_addr.s_addr));
	_addEnv("SCRIPT_NAME", "");
}

void Cgi::_addEnv(std::string key, std::string value)
{
	std::string tmp;

	tmp = key + "=" + value;
	_env[_envCnt] = new char[tmp.size() + 1];
	tmp.copy(_env[_envCnt], tmp.size());
	_env[_envCnt][tmp.size()] = '\0';
	_envCnt++;
	_env[_envCnt] = NULL;
}

void Cgi::cgiStart()
{
	Kqueue &kqueue = Kqueue::getInstance();
	std::string cgiScript;

	cgiScript = "./" + _response->getRoot() + "/" + _cgiInfo.cgiInfo;
	if (access(cgiScript.c_str(), F_OK))
		throw(_404_NOT_FOUND);
	if (access(cgiScript.c_str(), X_OK))
		throw(_403_FORBIDDEN);
	if (pipe(_reqFd))
		throw(_500_INTERNAL_SERVER_ERROR);
	if (pipe(_resFd))
		throw(_500_INTERNAL_SERVER_ERROR);
	fcntl(_reqFd[0], F_SETFL, O_NONBLOCK);
	fcntl(_reqFd[1], F_SETFL, O_NONBLOCK);
	fcntl(_resFd[0], F_SETFL, O_NONBLOCK);
	fcntl(_resFd[0], F_SETFL, O_NONBLOCK);
	_pid = fork();
	if (_pid == -1)
	{
		throw(_500_INTERNAL_SERVER_ERROR);
	}
	else if (_pid == 0)
	{
		char *args[2];

		dup2(_reqFd[0], STDIN_FILENO);
		dup2(_resFd[1], STDOUT_FILENO);
		close(_reqFd[0]);
		close(_resFd[1]);
		close(_reqFd[1]);
		close(_resFd[0]);

		args[0] = const_cast<char *>(cgiScript.c_str());
		args[1] = NULL;
		execve(cgiScript.c_str(), args, _env);
	}
	else
	{
		close(_reqFd[0]);
		close(_resFd[1]);
		kqueue.setFdset(_reqFd[1], CGI);
		kqueue.setFdset(_resFd[0], CGI);
		kqueue.addEvent(_reqFd[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, static_cast<void *>(this));
		kqueue.addEvent(_resFd[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, static_cast<void *>(this));
	}
}

void Cgi::writeBody()
{
	Kqueue &kqueue = Kqueue::getInstance();
	ssize_t res;

	res = write(_reqFd[1], _requestBody.c_str() + _lastPos, _requestBody.length() - _lastPos);
	_lastPos += res;
	if (_lastPos == _requestBodyLength)
	{
		kqueue.deleteEvent(_reqFd[1], EVFILT_WRITE, static_cast<void *>(this));
		kqueue.enableEvent(_resFd[0], EVFILT_READ, static_cast<void *>(this));
		Logger::serverWriteToCgi(_reqFd[1]);
		close(_reqFd[1]);
	}
}

void Cgi::readResponse()
{
	char buf[BUFFERSIZE + 1];
	ssize_t readSize;
	pid_t result;
	int status;

	while (true)
	{
		memset(buf, 0, BUFFERSIZE + 1);
		readSize = read(_resFd[0], buf, BUFFERSIZE);
		if (readSize == -1)
			return;
		else if (readSize == 0)
			break;
		_cgiResponse += buf;
	}
	result = waitpid(_pid, &status, WNOHANG);
	switch (result)
	{
	case 0:
		return;
	default:
		Kqueue &kqueue = Kqueue::getInstance();

		deleteCgiEvent();

		if (WIFEXITED(status) == true && WEXITSTATUS(status) == true)
			throw(_500_INTERNAL_SERVER_ERROR);
		else if (_cgiResponse.find("Status: ") != std::string::npos)
		{
			_cgiResponse = "HTTP/1.1 200 OK\r\nContent-Length: " + ft_itos(_cgiResponse.substr(_cgiResponse.find("\r\n\r\n") + 4).length()) + "\r\nContent-Type: text/html; charset=utf-8" + _cgiResponse.substr(_cgiResponse.find("\r\n\r\n"));
			_response->setResponse(_cgiResponse);
		}
		else
			_response->setResponse(_cgiResponse);
		kqueue.enableEvent(_clientSock, EVFILT_WRITE, static_cast<void *>(_client));
		_client->setCgi(NULL);
		Logger::serverReadFromCgi(_resFd[0]);
		delete this;
	}
}

void Cgi::deleteCgiEvent()
{
	Kqueue &kqueue = Kqueue::getInstance();

	kqueue.deleteEvent(_resFd[0], EVFILT_READ, static_cast<void *>(this));
	kqueue.deleteEvent(_reqFd[1], EVFILT_WRITE, static_cast<void *>(this));
	kqueue._deleteFdType(_reqFd[1]);
	kqueue._deleteFdType(_resFd[0]);
	close(_reqFd[1]);
	close(_resFd[0]);
}
