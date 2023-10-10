#include "Cgi.hpp"

Cgi::Cgi(Request *request, Response *response, uintptr_t socket)
{
	_request = request;
	_response = response;
	_clientSock = socket;
	_envCnt = 0;
	_reqFd[0] = 0;
	_reqFd[1] = 0;
	_resFd[0] = 0;
	_resFd[1] = 0;
	_pid = 0;
	_env = new char *[ENVMAXSIZE];
	_body.str(request->getParsedRequest()._body);
	_cgiExec = mapFind(_response->getLocationInfo()->getLocationInfo(), "cgi_exec");
	_cgiPath = "." + _response->getRoot() + mapFind(_response->getLocationInfo()->getLocationInfo(), "cgi_path");
	_makeEnvList(_clientSock);
}

Cgi::~Cgi()
{
}

uintptr_t Cgi::getClientSock() { return (_clientSock); }
Response *Cgi::getResponse() { return (_response); }
pid_t Cgi::getPid() { return (_pid); }

void Cgi::_makeEnvList(uintptr_t clntSock)
{
	struct sockaddr_in clnt;
	socklen_t clntSockLen = sizeof(clnt);
	port_t port;

	getsockname(clntSock, (sockaddr *)&clnt, &clntSockLen);
	port = ntohs(clnt.sin_port);

	_addEnv("SERVER_SOFTWARE", "webserv/0.1");
	_addEnv("SERVER_PROTOCOL", "HTTP/1.1");
	_addEnv("SERVER_PORT", ft_itos(port));
	_addEnv("SERVER_NAME", mapFind(_response->getServerInfo()->getServerInfo(), "server_name"));
	_addEnv("GETWAY_INTERFACE", "CGI/1.1");
	_addEnv("REQUEST_METHOD", _request->getParsedRequest()._method);
	_addEnv("REMOTE_ADDR", ft_itos(clnt.sin_addr.s_addr));
	_addEnv("CONTENT_TYPE", _request->getParsedRequest()._contentType);
	_addEnv("CONTENT_LENGTH", ft_itos(_request->getParsedRequest()._body.size()));
	_addEnv("SCRIPT_NAME", _cgiPath);
	_addEnv("PATH_INFO", _response->getRoot() + _request->getParsedRequest()._location);
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

	if (access(_cgiPath.c_str(), F_OK))
		throw(_404_NOT_FOUND);
	if (access(_cgiPath.c_str(), X_OK))
		throw(_403_FORBIDDEN);
	if (pipe(_reqFd))
		throw(_500_INTERNAL_SERVER_ERROR);
	if (pipe(_resFd))
		throw(_500_INTERNAL_SERVER_ERROR);
	_pid = fork();
	if (_pid == -1)
	{
		throw(_500_INTERNAL_SERVER_ERROR);
	}
	else if (_pid == 0)
	{
		char *args[3];

		dup2(_reqFd[0], STDIN_FILENO);
		dup2(_resFd[1], STDOUT_FILENO);
		close(_reqFd[0]);
		close(_resFd[1]);
		close(_reqFd[1]);
		close(_resFd[0]);

		args[0] = &_cgiExec[0];
		args[1] = &_cgiPath[0];
		args[2] = NULL;
		execve(_cgiExec.c_str(), args, _env);
		throw(_403_FORBIDDEN);
	}
	else
	{
		fcntl(_reqFd[1], F_SETFL, O_NONBLOCK);
		fcntl(_resFd[0], F_SETFL, O_NONBLOCK);
		close(_reqFd[0]);
		close(_resFd[1]);
		kqueue.setFdset(_reqFd[1], CGI);
		kqueue.setFdset(_resFd[0], CGI);
		kqueue.addEvent(_reqFd[1], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, static_cast<void *>(this));
		kqueue.addEvent(_resFd[0], EVFILT_READ, EV_ADD | EV_DISABLE, 0, 0, static_cast<void *>(this));
	}
}

void Cgi::writeBody()
{
	char buf[BUFFERSIZE + 1];
	Kqueue &kqueue = Kqueue::getInstance();

	std::streampos startPos = _body.tellg();
	std::cout << "start tellg : " << startPos << std::endl;
	memset(static_cast<void *>(buf), 0, BUFFERSIZE + 1);
	_body.read(buf, BUFFERSIZE);
	std::streampos endPos = _body.tellg();
	if (endPos == -1)
		endPos = _body.str().length();
	std::cout << "end tellg : " << endPos << std::endl;
	if (_body.eof() == true)
	{
		std::cout << buf << std::endl;
		kqueue.disableEvent(_reqFd[1], EVFILT_WRITE, static_cast<void *>(this));
		kqueue.enableEvent(_resFd[0], EVFILT_READ, static_cast<void *>(this));
		write(_reqFd[1], buf, endPos - startPos);
		close(_reqFd[1]);
		return;
	}
	if (_body.fail() == true)
	{
		throw(_500_INTERNAL_SERVER_ERROR);
	}
	write(_reqFd[1], buf, BUFFERSIZE);
}

void Cgi::readResponse()
{
	char buf[BUFFERSIZE + 1];
	ssize_t readSize;
	pid_t result;

	memset(static_cast<void *>(buf), 0, BUFFERSIZE);
	readSize = read(_resFd[0], buf, BUFFERSIZE);
	std::cout << "ReadSize : " << readSize << std::endl;
	std::cout << "===========buf===========" << std::endl;
	std::cout << buf << std::endl;
	if (readSize == -1)
		throw(_500_INTERNAL_SERVER_ERROR);
	else if (readSize == 0)
	{
		result = waitpid(_pid, NULL, WNOHANG);
		std::cout << "Result : " << result << std::endl;
		switch (result)
		{
		case 0:
			return;
		case -1:
			throw(_500_INTERNAL_SERVER_ERROR);
			break;
		default:
			deleteCgiEvent();
			_response->setResponse(_cgiResponse);
			delete this;
			break;
		}
	}
	else
		_cgiResponse += buf;
}

void Cgi::deleteCgiEvent()
{
	Kqueue &kqueue = Kqueue::getInstance();

	// kqueue.addEvent(_reqFd[1], EVFILT_WRITE, EV_DELETE, 0, 0, static_cast<void *>(this));
	// kqueue.addEvent(_resFd[0], EVFILT_READ, EV_DELETE, 0, 0, static_cast<void *>(this));
	kqueue.deleteEvent(_resFd[0], EVFILT_READ, static_cast<void *>(this));
	kqueue.deleteEvent(_reqFd[1], EVFILT_WRITE, static_cast<void *>(this));
	kqueue._deleteFdType(_reqFd[1]);
	kqueue._deleteFdType(_resFd[0]);
	close(_reqFd[1]);
	close(_resFd[0]);
}
