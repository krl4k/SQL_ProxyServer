//
// Created by Foster Grisella on 4/30/21.
//

#include <iostream>
#include "Client.hpp"

Client::Client() {

}

Client::~Client() {
	close(_socket);
	close(_database_Socket);
}

Client::Client(int clientSocket, const sockaddr_in &addr) : _socket(clientSocket), _addr(addr) {
	if ((_database_Socket = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		std::cerr << "fatal error! socket!" << std::endl;
		exit(-1);
	}
	if (connect(_database_Socket, (struct sockaddr *)&addr, sizeof addr) < 0) {
		std::cerr << "fatal error! connect!" << std::endl;
		exit(-1);
	}
	state = State::READ_FROM_CLIENT;
}

int Client::getSocket() const {
	return _socket;
}

int Client::getState() const {
	return state;
}

void Client::setState(int state) {
	Client::state = state;
}

void Client::setBody(char *body, int bodySize) {
	std::cout << "len = " << bodySize << ", response:\n" << std::endl;
	for (int i = 0; i < bodySize; ++i) {
		if (i > 4 && i < bodySize - 1)
			std::cout << body[i];
		_body.push_back(body[i]);
	}
	std::cout << std::endl;
}

int Client::getDatabaseSocket() const {
	return _database_Socket;
}

char *Client::getBody() const {
	std::cout << "get Body:" << std::endl;
	char *buf = new char[_body.size()];
	for (int i = 0; i < _body.size(); ++i) {
		buf[i] = _body[i];
		std::cout << buf[i];
	}
	std::cout << std::endl;
	return buf;

}

int Client::getBodySize() const {
	return static_cast<int>(_body.size());
}

void Client::clearBody() {
	_body.clear();
}
