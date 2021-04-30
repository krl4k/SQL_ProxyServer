//
// Created by Foster Grisella on 4/30/21.
//

#include <fstream>
#include <sys/fcntl.h>
#include <unistd.h>
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

    if ((_listenSocketFd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
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
    if (fcntl(_listenSocketFd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "fcntl error" << std::endl;
        if (close(_listenSocketFd) < 0){
            std::cerr << "error close" << std::endl;
        }
        return -3;
    }
    if (listen(_listenSocketFd, 1024) < 0){
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
    fd_set readFdSet, writeFdSet;
    _maxFdSize = _listenSocketFd;

    while (true) {
       	std::cout << "\nWaiting for connection! " <<"Clients count = " << _client.size() << std::endl;
        initSocketSet(readFdSet, writeFdSet);

        select(_maxFdSize + 1, &readFdSet, &writeFdSet, NULL, NULL);

        acceptNewClient(readFdSet);
        handler(readFdSet, writeFdSet);
    }
}

void Server::initSocketSet(fd_set &readFdSet, fd_set &writeFdSet) {
    FD_ZERO(&readFdSet);
    FD_ZERO(&writeFdSet);

    FD_SET(_listenSocketFd, &readFdSet);
    for (size_t i = 0; i < _client.size(); ++i) {
        FD_SET(_client[i]->getSocket(), &readFdSet);
        if (_client[i]->getState() != Client::State::READ_FROM_CLIENT){
            FD_SET(_client[i]->getSocket(), &writeFdSet);
        }
        if (_client[i]->getSocket() > _maxFdSize)
            _maxFdSize = _client[i]->getSocket();
    }
}

void Server::acceptNewClient(fd_set &readFdSet) {
    if (FD_ISSET(_listenSocketFd, &readFdSet)){
		int clientSocket;
		if ((clientSocket = accept(getListenSocketFd(), nullptr, nullptr)) < 0){
			std::cerr << "Client accept error!" << std::endl;
		}
		Client *newClient = new Client(clientSocket, _databaseAddr);
        _client.push_back(newClient);
        if (newClient->getSocket() > _maxFdSize)
            _maxFdSize = newClient->getSocket();
    }
}

void Server::handler(fd_set &readFdSet, fd_set &writeFdSet) {
	for (auto clientIter = _client.begin(); !_client.empty() && clientIter != _client.end(); ++clientIter) {
		int fd = (*clientIter)->getSocket();
		if (FD_ISSET(fd, &readFdSet) && (*clientIter)->getState() == Client::State::READ_FROM_CLIENT) {
			readRequestFromClient((*clientIter));
		}
		if (FD_ISSET(fd, &writeFdSet) and (*clientIter)->getState() == Client::State::SEND_TO_DATABASE){
			sendRequestToDB((*clientIter));
		}
		if (FD_ISSET(fd, &readFdSet) and (*clientIter)->getState() == Client::State::READ_FROM_DATABASE) {
			readRequestFromDataBase((*clientIter));
		}
		if (FD_ISSET(fd, &writeFdSet) and (*clientIter)->getState() == Client::State::SEND_TO_CLIENT) {
			sendResponseToClient((*clientIter));
		}
		if ((*clientIter)->getState() == Client::State::CLOSE_CONNECTION) {
			delete (*clientIter);
			_client.erase(clientIter);
		}
	}
}

void Server::initDbAddr(){
	bzero(&_databaseAddr, sizeof _databaseAddr);
	_databaseAddr.sin_family = AF_INET;
	_databaseAddr.sin_addr.s_addr = inet_addr(_dbHost.c_str());
	_databaseAddr.sin_port = htons(_dbPort);
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

void Server::readRequestFromClient(Client *&pClient) {
	char buf[BUFSIZ + 1];
	int ret = 0;
	//todo check with null terminated string
	ret = recv(pClient->getSocket(), buf, BUFSIZ, 0);
	if (ret <= 0){
		pClient->setState(Client::State::CLOSE_CONNECTION);
		return;
	}
	pClient->setBody(buf, ret);
	pClient->setState(Client::State::SEND_TO_DATABASE);

	// todo add error log func
	addToLogFile(pClient);
}

void Server::sendRequestToDB(Client *&pClient) {

	send(pClient->getDatabaseSocket(), pClient->getBody(), pClient->getBodySize(), 0);
	pClient->clearBody();
	pClient->setState(Client::State::READ_FROM_DATABASE);
}

void Server::readRequestFromDataBase(Client *&pClient) {
	char buf[BUFSIZ + 1];
	int ret = 0;
	ret = recv(pClient->getSocket(), buf, BUFSIZ, 0);
	if (ret <= 0){
		pClient->setState(Client::State::CLOSE_CONNECTION);
		return;
	}
	pClient->setBody(buf, ret);
	pClient->setState(Client::State::SEND_TO_CLIENT);
}

void Server::sendResponseToClient(Client *&pClient) {
	send(pClient->getDatabaseSocket(), pClient->getBody(), pClient->getBodySize(), 0);
	pClient->clearBody();
	pClient->setState(Client::State::CLOSE_CONNECTION);
}

void Server::addToLogFile(Client *&pClient) {
	write(_logFileFd, pClient->getBody(), pClient->getBodySize());
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
