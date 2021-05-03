//
// Created by kirill on 03.05.2021.
//

#include "ProxyServer.hpp"

//Серверные TCP функции в asio выполняет объект класса boost::asio::ip::tcp::acceptor.
// Перед тем, как открыть соединение, необходимо настроить, откуда ждать этого соединения.
// Для этого используется объект класса boost::asio::ip::tcp::endpoint.
// Если endpoint используется в связке с acceptor, то endpoint хранит адреса и порт откуда ждать соединения.

ProxyServer::ProxyServer(boost::asio::io_service &ioService, const std::string &logFileName,
						 const std::string &serverHost, uint16_t serverPort, const std::string &dbHost,
						 uint16_t dbPort)
		: _ioService(ioService), logFileName(logFileName),
		_serverHost(serverHost), _serverPort(serverPort),
		  _dbHost(dbHost), _dbPort(dbPort),
		  _selfAddress(boost::asio::ip::address_v4::from_string(_serverHost)),
		  _acceptor(ioService,boost::asio::ip::tcp::endpoint(_selfAddress, _serverPort)) {}
