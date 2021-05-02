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
		SEND_TO_DATABASE,
		CLOSE_CONNECTION
	};

	Client(int clientSocket, const sockaddr_in &addr);

	void setState(int state);

	int getSocket() const;

	virtual ~Client();

	int getState() const;

	char *getBody() const;
	int 	getBodySize() const;

	void setBody(char *body, int bodySize);

	int getDatabaseSocket() const;

	void clearBody();

private:
    int _socket;
	sockaddr_in _addr;
	int _database_Socket;

    std::vector<char> _body;
    int   state;

	Client();
	Client(const Client &other);
	Client &operator=(const Client &other);

};

#endif //SQL_TCP_SERVER_CLIENT_HPP
