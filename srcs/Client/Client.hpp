//
// Created by Foster Grisella on 4/30/21.
//

#ifndef SQL_TCP_SERVER_CLIENT_HPP
#define SQL_TCP_SERVER_CLIENT_HPP


#include <vector>
#include <netinet/in.h>
#include <unistd.h>


class Client {
public:
	enum State {
		START,
		READ_FROM_CLIENT,
		READ_FROM_DATABASE,
		SEND_TO_CLIENT,
		SEND_TO_DATABASE
	};


	Client(int clientSocket, const sockaddr_in &addr);

	int getSocket() const;

	virtual ~Client();

	int getState() const;

private:
    int _socket;
	sockaddr_in _addr;
	int _database_Socket;

    std::vector<char> body;
    int   state;

	Client();
	Client(const Client &other);
	Client &operator=(const Client &other);

};


#endif //SQL_TCP_SERVER_CLIENT_HPP
