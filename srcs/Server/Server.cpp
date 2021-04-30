//
// Created by Foster Grisella on 4/30/21.
//

#include <fstream>
#include <sys/fcntl.h>
#include <unistd.h>
#include "Server.hpp"

Server::Server(char *filename) : _serverPort(0), _serverHost(""), _dbPort(0), _dbHost("") {
    std::string fileName;
    if (!filename){
        fileName = "../configs/config.conf";
    }
    try {
        parseConfig(fileName);
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
    if (createListenSocket() < 0){
        throw std::exception();
    }
	initDbAddr();

    lifeStyle();
}

int Server::createListenSocket() {
    int listenFd;
    struct sockaddr_in servaddr;

    if ((listenFd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        std::cerr << "fatal error! socket!" << std::endl;
        return -1;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(_serverHost.c_str());
    servaddr.sin_port = htons(_serverPort);
    int yes = 1;
    if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        std::cerr << "error setsockopt" << std::endl;
        if (close(listenFd) < 0){
            std::cerr << "error close" << std::endl;
        }
        return -2;
    }
    if ((bind(listenFd, (struct sockaddr * )&servaddr, sizeof servaddr)) < 0){
        std::cerr << "bind error" << strerror(errno) << std::endl;
        if (close(listenFd) < 0){
            std::cerr << "error close" << std::endl;
        }
        return -2;
    }
    if (fcntl(listenFd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "fcntl error" << std::endl;
        if (close(listenFd) < 0){
            std::cerr << "error close" << std::endl;
        }
        return -3;
    }
    if (listen(listenFd, 1024) < 0){
        std::cerr << "listen error!" << std::endl;
        if (close(listenFd) < 0){
            std::cerr << "error close" << std::endl;
        }
        return -4;
    }
    _listenSocketFd = listenFd;
    _databaseAddr 	= servaddr;
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
	for (size_t i = 0; i < _client.size(); ++i) {
		int fd = _client[i]->getSocket();
		if (FD_ISSET(fd, &readFdSet) && _client[i]->getState() == Client::State::READ_FROM_CLIENT) {
			readRequestFromClient(_client[i]);
		}
		if (FD_ISSET(fd, &writeFdSet) and _client[i]->getState() == Client::State::SEND_TO_DATABASE){
			sendRequestToDB(_client[i]);
		}
		if (FD_ISSET(fd, &writeFdSet) and _client[i]->getState() == Client::State::READ_FROM_DATABASE) {
			readRequestFromDataBase(_client[i]);
		}
		if (FD_ISSET(fd, &writeFdSet) and _client[i]->getState() == Client::State::READ_FROM_DATABASE) {
		}
		if (_client[i]->getState() == Client__State__CLOSE) {
			std::vector<Client *>::iterator it = _client.begin() + (int)i;
			delete _client[i];
			_client.erase(it);
		}
	}
}

void Server::initDbAddr(){
	bzero(&_databaseAddr, sizeof _databaseAddr);
	_databaseAddr.sin_family = AF_INET;
	_databaseAddr.sin_addr.s_addr = inet_addr(_serverHost.c_str());
	_databaseAddr.sin_port = htons(_serverPort);
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
