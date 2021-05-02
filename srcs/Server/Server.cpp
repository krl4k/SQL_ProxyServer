//
// Created by Foster Grisella on 4/30/21.
//

#include <fstream>
#include <sys/fcntl.h>
#include <unistd.h>
#include <cstring>
#include "Server.hpp"

Server::Server(char *filename) : _serverPort(0), _serverHost(""), _dbPort(0), _dbHost("") {
    try {
        parseConfig(filename);
		if (createListenSocket() < 0)
			throw std::exception();
		initDbAddr();
	} catch (std::exception &exception){
        throw std::runtime_error(exception.what());
    }
}

void Server::parseConfig(std::string fileName) {
    std::ifstream file(fileName);
    if (!file.is_open())
        throw std::runtime_error("File not found!!!");

    std::string strings[5];
    for (int i = 0; i < 5; ++i) {
        std::getline(file, strings[i]);
    }
    file.close();
    if ("proxy_host: " == strings[0].substr(0, 12)){
		_serverHost = std::string(strings[0], 12);
    }
    if ("proxy_port: " == strings[1].substr(0, 12)){
        std::string port = strings[1].substr(12);
        if ((_serverPort = convertPort(port)) < 0)
			throw std::exception();
    }
	if ("database_host: " == strings[2].substr(0, 15)){
		_dbHost = std::string(strings[2], 15);
	}
	if ("database_port: " == strings[3].substr(0, 15)){
		std::string port = strings[3].substr(15);
		if ((_dbPort = convertPort(port)) < 0)
			throw std::exception();
	}
    if ("log_file: " == strings[4].substr(0, 10)){
        std::string logFile(strings[4], 10);
        if ((_logFileFd = open(logFile.c_str(), O_WRONLY | O_TRUNC)) < 0){
            throw std::runtime_error("logFile error not opened!!!");
        }
    }
    if (_serverHost.empty() || _dbHost.empty()){
		throw std::runtime_error("Host error!!!");
    }
    file.close();
}

void Server::start() {
    lifeStyle();
}

int Server::createListenSocket() {
    struct sockaddr_in servaddr;

    if ((_listenSocketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        std::cerr << "fatal error! socket!" << std::endl;
        return -1;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(_serverHost.c_str());
    servaddr.sin_port = htons(_serverPort);
    int yes = 1;
    if (setsockopt(_listenSocketFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        std::cerr << "error setsockopt" << std::endl;
        if (close(_listenSocketFd) < 0){
            std::cerr << "error close" << std::endl;
        }
        return -2;
    }
    if ((bind(_listenSocketFd, (struct sockaddr * )&servaddr, sizeof servaddr)) < 0){
        std::cerr << "bind error" << strerror(errno) << std::endl;
        if (close(_listenSocketFd) < 0){
            std::cerr << "error close" << std::endl;
        }
        return -2;
    }
//    if (fcntl(_listenSocketFd, F_SETFL, O_NONBLOCK) < 0) {
    if (fcntl(_listenSocketFd, F_SETFL, fcntl(_listenSocketFd, F_GETFL, 0) | O_NONBLOCK) < 0) {
        std::cerr << "fcntl error" << std::endl;
        if (close(_listenSocketFd) < 0){
            std::cerr << "error close" << std::endl;
        }
        return -3;
    }
    if (listen(_listenSocketFd, 128) < 0){
        std::cerr << "listen error!" << std::endl;
        if (close(_listenSocketFd) < 0){
            std::cerr << "error close" << std::endl;
        }
        return -4;
    }
    return _listenSocketFd;
}

[[noreturn]]
void Server::lifeStyle() {

	while (true) {
       	std::cout << "\nWaiting for connection! " <<"Clients count = " << _client.size() << std::endl;
		_maxFdSize = _listenSocketFd;
		initSocketSet();

		select(_maxFdSize + 1, &_readFdSet, &_writeFdSet, NULL, NULL);

		acceptNewClient();

        handler();
    }
}

void Server::initSocketSet() {
	_maxFdSize = getListenSocketFd();

	FD_ZERO(&_readFdSet);
	FD_ZERO(&_writeFdSet);

    FD_SET(_listenSocketFd, &_readFdSet);
	for (int i = 0; i < _client.size(); ++i){
		Client * client = _client[i];
		if (client->getState() == Client::State::READ_FROM_CLIENT){
			FD_SET(client->getSocket(), &_readFdSet);
			setMaxFdSize(std::max(client->getSocket(), _maxFdSize));

		}else if (client->getState() == Client::State::SEND_TO_DATABASE){
			FD_SET(client->getDatabaseSocket(), &_writeFdSet);
			setMaxFdSize(std::max(client->getDatabaseSocket(), _maxFdSize));

		}else if (client->getState() == Client::State::READ_FROM_DATABASE){
			FD_SET(client->getDatabaseSocket(), &_readFdSet);
			setMaxFdSize(std::max(client->getDatabaseSocket(), _maxFdSize));

		}else if (client->getState() == Client::State::SEND_TO_CLIENT){
			FD_SET(client->getSocket(), &_writeFdSet);
			setMaxFdSize(std::max(client->getSocket(), _maxFdSize));
		}
	}
}

void Server::acceptNewClient() {
    if (FD_ISSET(_listenSocketFd, &_readFdSet))
    {
		int clientSocket;
		if ((clientSocket = accept(getListenSocketFd(), nullptr, nullptr)) < 0){
			std::cerr << "Client accept error!" << std::endl;
		}
		fcntl(clientSocket, F_SETFL, O_NONBLOCK);
		Client *newClient = new Client(clientSocket, _databaseAddr);
        _client.push_back(newClient);
        if (newClient->getSocket() > _maxFdSize)
            _maxFdSize = newClient->getSocket();
    }
}

void Server::handler() {
	for (int i = 0; i < _client.size(); ++i){
		if (FD_ISSET((_client[i])->getSocket(), &_readFdSet) and (_client[i])->getState() == Client::State::READ_FROM_CLIENT) {
			readRequestFromClient((_client[i]));
		}
		if (FD_ISSET((_client[i])->getDatabaseSocket(), &_writeFdSet) and (_client[i])->getState() == Client::State::SEND_TO_DATABASE){
			sendRequestToDB((_client[i]));
		}
		if (FD_ISSET((_client[i])->getDatabaseSocket(), &_readFdSet) and (_client[i])->getState() == Client::State::READ_FROM_DATABASE) {
			readRequestFromDataBase((_client[i]));
		}
		if (FD_ISSET((_client[i])->getSocket(), &_writeFdSet) and (_client[i])->getState() == Client::State::SEND_TO_CLIENT) {
			sendResponseToClient((_client[i]));
		}
		if ((_client[i])->getState() == Client::State::CLOSE_CONNECTION) {
			auto it = _client.begin() + i;
			delete *it;
			_client.erase(it);
		}
	}
}

void Server::initDbAddr(){
	bzero(&_databaseAddr, sizeof _databaseAddr);
	_databaseAddr.sin_family = AF_INET;
	_databaseAddr.sin_addr.s_addr = inet_addr(_dbHost.c_str());
	_databaseAddr.sin_port = htons(_dbPort);
}


void Server::readRequestFromClient(Client *&pClient) {
	std::cout << MAGENTA << "Read request from client!" << RESET << std::endl;
	char buf[BUFSIZ];
	int ret = 0;

	ret = recv(pClient->getSocket(), buf, BUFSIZ, 0);
//	std::cout << "read!:" << std::endl;
//	for (int i = 0; i < ret; ++i)
//	{
//		std::cout << buf[i];
//	}
	std::cout << std::endl;
	if (ret <= 0){
		pClient->setState(Client::State::CLOSE_CONNECTION);
		return;
	}
	buf[ret] = 0;
	pClient->setBody(buf, ret);
	if (buf[4] == 3)
		addToLogFile(pClient);

	pClient->setState(Client::State::SEND_TO_DATABASE);
}

void Server::sendRequestToDB(Client *&pClient) {
	std::cout << YELLOW << "Send request to DB!" << RESET << std::endl;
	char * temp = pClient->getBody();
	send(pClient->getDatabaseSocket(), temp, pClient->getBodySize(), 0);
	free(temp);
	pClient->clearBody();

	pClient->setState(Client::State::READ_FROM_DATABASE);
}

void Server::readRequestFromDataBase(Client *&pClient) {
	std::cout << YELLOW << "Read response from DB!" << RESET << std::endl;

	char buf[BUFSIZ];
	int ret = recv(pClient->getDatabaseSocket(), buf, BUFSIZ, 0);
	if (ret <= 0){
		pClient->setState(Client::State::CLOSE_CONNECTION);
		return;
	}
	pClient->setBody(buf, ret);
	pClient->setState(Client::State::SEND_TO_CLIENT);
}

void Server::sendResponseToClient(Client *&pClient) {
	std::cout << YELLOW << "Send response to Client!" << RESET << std::endl;
	send(pClient->getSocket(), pClient->getBody(), pClient->getBodySize(), 0);
	pClient->clearBody();

	pClient->setState(Client::State::READ_FROM_CLIENT);
}

void Server::addToLogFile(Client *&pClient) {
	std::string date;
	time_t rawtime;
	time(&rawtime);
	date = ctime(&rawtime);
	date.erase(date.size() - 1);
	write(_logFileFd, "\n-----------------------------------------\n", 43);
	write(_logFileFd, "Data: ", 5);
	write(_logFileFd, date.c_str(), date.size());
	write(_logFileFd, "\n", 1);
	char *log = pClient->getBody();
	for (int i = 5; i < pClient->getBodySize() - 1; ++i) {
		write(_logFileFd, &log[i], 1);
	}
	write(_logFileFd, "\n-----------------------------------------\n", 43);
}

uint16_t Server::convertPort(const std::string &port) {
	uint16_t p;
	try{
		p = static_cast<uint16_t>(std::stoi(port));
	}
	catch (std::exception &e){
		std::cout << "Port setting error" << std::endl;
		return -1;
	}
	return p;
}


int Server::getListenSocketFd() const {
	return _listenSocketFd;
}

const std::string &Server::getServerHost() const {
	return _serverHost;
}

void Server::setServerHost(const std::string &serverHost) {
	_serverHost = serverHost;
}

uint16_t Server::getServerPort() const {
	return _serverPort;
}

void Server::setServerPort(uint16_t serverPort) {
	_serverPort = serverPort;
}

const std::string &Server::getDbHost() const {
	return _dbHost;
}

void Server::setDbHost(const std::string &dbHost) {
	_dbHost = dbHost;
}

uint16_t Server::getDbPort() const {
	return _dbPort;
}

void Server::setDbPort(uint16_t dbPort) {
	_dbPort = dbPort;
}

void Server::setMaxFdSize(int maxFdSize) {
	_maxFdSize = maxFdSize;
}
