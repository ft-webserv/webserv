#include "Cgi.hpp"

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
	char buf[100];
	// _cgiExec = "/usr/bin/python3";
	_cgiExec = getcwd(buf, 100) + _response->getRoot() + _cgiInfo.cgiExec;
	_cgiPath = "." + _response->getRoot() + _cgiInfo.cgiPath;
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
// bool Cgi::getIsCgiFin() { return (_isCgiFin); }

void Cgi::_makeEnvList(uintptr_t clntSock)
{
	struct sockaddr_in clnt;
	socklen_t clntSockLen = sizeof(clnt);
	port_t port;

	getsockname(clntSock, (sockaddr *)&clnt, &clntSockLen);
	port = ntohs(clnt.sin_port);
	for ()
	{
		std::for_each();
	}
	_addEnv("CONTENT_TYPE", _request->getParsedRequest()._contentType);
	_addEnv("CONTENT_LENGTH", ft_itos(_request->getParsedRequest()._body.size()));
	_addEnv("SERVER_NAME", mapFind(_response->getServerInfo()->getServerInfo(), "server_name"));
	_addEnv("SERVER_PROTOCOL", "HTTP/1.1");
	char buf[100];
	_addEnv("PATH_INFO", getcwd(buf, 100) + _response->getRoot() + _cgiInfo.cgiExec);
	_addEnv("SERVER_SOFTWARE", "webserv/0.1");
	_addEnv("SERVER_PORT", ft_itos(port));
	_addEnv("GETWAY_INTERFACE", "CGI/1.1");
	_addEnv("REQUEST_METHOD", _request->getParsedRequest()._method);
	_addEnv("REMOTE_ADDR", ft_itos(clnt.sin_addr.s_addr));
	// _addEnv("SCRIPT_NAME", getcwd(buf, 100) + _response->getRoot() + _cgiInfo.cgiExec);
}

void Cgi::_addEnv(std::string key, std::string value)
{
	std::string tmp;

	tmp = key + "=" + value;
	_env[_envCnt] = new char[tmp.size() + 1];
	tmp.copy(_env[_envCnt], tmp.size());
	_env[_envCnt][tmp.size()] = '\0';
	std::cout << _env[_envCnt] << std::endl;
	_envCnt++;
	_env[_envCnt] = NULL;
}

void Cgi::cgiStart()
{
	Kqueue &kqueue = Kqueue::getInstance();

	std::cout << "CGI INFO: 1 " << _cgiExec << std::endl;
	std::cout << "CGI INFO: 2 " << _cgiPath << std::endl;
	if (access(_cgiExec.c_str(), F_OK))
		throw(_404_NOT_FOUND);
	if (access(_cgiExec.c_str(), X_OK))
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

		args[0] = new char[100];
		args[1] = new char[100];
		// args[2] = new char[100];
		// args[0] = "/usr/bin/python3\0";
		// args[1] = "./cgi-bin/post.py\0";
		// args[2] = NULL;
		_cgiExec.copy(args[0], _cgiExec.size() + 1);
		args[0][_cgiExec.size()] = '\0';
		args[1] = NULL;
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
		kqueue.addEvent(_resFd[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, static_cast<void *>(this));
	}
}

void Cgi::writeBody()
{
	Kqueue &kqueue = Kqueue::getInstance();
	ssize_t res;

	std::cout << "REQUEST BODY SIZE : " << _requestBodyLength << std::endl;
	std::cout << "_lastPos : " << _lastPos << std::endl;
	std::cout << "CGI INFO: 1 " << _cgiExec << std::endl;
	std::cout << "CGI INFO: 2 " << _cgiPath << std::endl;
	if (_request->getParsedRequest()._method == "GET")
	{
		kqueue.deleteEvent(_reqFd[1], EVFILT_WRITE, static_cast<void *>(this));
		kqueue.enableEvent(_resFd[0], EVFILT_READ, static_cast<void *>(this));
		close(_reqFd[1]);
	}
	res = write(_reqFd[1], _requestBody.c_str() + _lastPos, _requestBody.length() - _lastPos);
	std::cout << "RESULT : " << res << std::endl;
	// if (res == -1)
	// 	throw(_500_INTERNAL_SERVER_ERROR);
	_lastPos += res;
	if (_lastPos == _requestBodyLength)
	{
		kqueue.deleteEvent(_reqFd[1], EVFILT_WRITE, static_cast<void *>(this));
		kqueue.enableEvent(_resFd[0], EVFILT_READ, static_cast<void *>(this));
		close(_reqFd[1]);
	}
}

void Cgi::readResponse(struct kevent *event)
{
	std::string buf;
	ssize_t readSize;
	pid_t result;

	buf.clear();
	buf.resize(event->data);
	readSize = read(_resFd[0], &buf[0], event->data);
	// std::cout << "ReadSize : " << readSize << std::endl;
	// std::cout << "CGIRESPONSE LENGTH : " << _cgiResponse.length() << std::endl;
	// std::cout << buf << std::endl;
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
			Kqueue &kqueue = Kqueue::getInstance();

			deleteCgiEvent();

			if (_cgiResponse.find("Status: ") != std::string::npos)
			{
				// std::cout << "%%%%%%%%%%%%%%%%%%%%" << std::endl;
				std::cout << _cgiResponse.substr(0, 200) << std::endl;
				// std::cout << "%%%%%%%%%%%%%%%%%%%%" << std::endl;
				if (_request->getParsedRequest()._method == "GET")
					_cgiResponse = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
				else
				{
					_cgiResponse = "HTTP/1.1 200 OK\r\nContent-Length: " + ft_itos(_cgiResponse.substr(_cgiResponse.find("\r\n\r\n") + 4).length()) + "\r\nContent-Type: text/html; charset=utf-8" + _cgiResponse.substr(_cgiResponse.find("\r\n\r\n"));
				}
				_response->setResponse(_cgiResponse);
			}
			else
				_response->setResponse(_cgiResponse);
			kqueue.enableEvent(_clientSock, EVFILT_WRITE, static_cast<void *>(_client));
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

	kqueue.deleteEvent(_resFd[0], EVFILT_READ, static_cast<void *>(this));
	kqueue.deleteEvent(_reqFd[1], EVFILT_WRITE, static_cast<void *>(this));
	kqueue._deleteFdType(_reqFd[1]);
	kqueue._deleteFdType(_resFd[0]);
	close(_reqFd[1]);
	close(_resFd[0]);
}
