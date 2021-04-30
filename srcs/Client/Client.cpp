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
	state = State::START;
}

int Client::getSocket() const {
	return _socket;
}

int Client::getState() const {
	return state;
}
