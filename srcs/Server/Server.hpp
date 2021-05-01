//
// Created by Foster Grisella on 4/30/21.
//

#ifndef UNTITLED1_SERVER_H
#define UNTITLED1_SERVER_H

#include <iostream>
#include <Client/Client.hpp>
#include <vector>
#include <netinet/in.h>

#if __APPLE__
#include <dns_util.h>
#elif __linux__
#include <arpa/inet.h>
#endif

#define BLACK "\x1B[30m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"
#define RESET "\x1B[0m"

class Server {
public:

    Server(char *filename);
    void start();


private:
    std::string _serverHost;
    uint16_t    _serverPort;
	std::string _dbHost;
	uint16_t    _dbPort;
	int         _logFileFd;

	fd_set _readFdSet;
	fd_set _writeFdSet;

    int									_listenSocketFd;

    struct sockaddr_in  				_databaseAddr;

    std::vector<Client *> 	_client;
    int						_maxFdSize;


	int getListenSocketFd() const;



	void parseConfig(std::string fileName);

//public:
	const std::string &getServerHost() const;

	void setServerHost(const std::string &serverHost);

	uint16_t getServerPort() const;

	void setServerPort(uint16_t serverPort);

	const std::string &getDbHost() const;

	void setDbHost(const std::string &dbHost);

	uint16_t getDbPort() const;

	void setDbPort(uint16_t dbPort);

public:
	void setMaxFdSize(int maxFdSize);

private:

	int createListenSocket();

	[[noreturn]] void lifeStyle();

    void initSocketSet();

    void acceptNewClient();

    void handler();

	void initDbAddr();

	uint16_t convertPort(const std::string &port);

	void readRequestFromClient(Client *&pClient);

	void sendRequestToDB(Client *&pClient);

	void readRequestFromDataBase(Client *&pClient);

	void sendResponseToClient(Client *&pClient);

	void addToLogFile(Client *&pClient);
};


#endif //UNTITLED1_SERVER_H
