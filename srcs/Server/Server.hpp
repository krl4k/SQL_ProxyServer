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

    int									_listenSocketFd;

    struct sockaddr_in  				_databaseAddr;

    std::vector<Client *> 	_client;
    int						_maxFdSize;

public:
	int getListenSocketFd() const;

private:

	void parseConfig(std::string fileName);

public:
	const std::string &getServerHost() const;

	void setServerHost(const std::string &serverHost);

	uint16_t getServerPort() const;

	void setServerPort(uint16_t serverPort);

	const std::string &getDbHost() const;

	void setDbHost(const std::string &dbHost);

	uint16_t getDbPort() const;

	void setDbPort(uint16_t dbPort);

private:

	int createListenSocket();

	[[noreturn]] void lifeStyle();

    void initSocketSet(fd_set &set, fd_set &set1);

    void acceptNewClient(fd_set &readdSet);

    void handler(fd_set &readFdSet, fd_set &writeFdSet);

	void initDbAddr();

	uint16_t convertPort(const std::string &port);
};


#endif //UNTITLED1_SERVER_H
