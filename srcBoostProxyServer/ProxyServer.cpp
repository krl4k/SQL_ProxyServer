//
// Created by kirill on 03.05.2021.
//

#include "ProxyServer.hpp"

//Серверные TCP функции в asio выполняет объект класса boost::asio::ip::tcp::acceptor.
// Перед тем, как открыть соединение, необходимо настроить, откуда ждать этого соединения.
// Для этого используется объект класса boost::asio::ip::tcp::endpoint.
// Если endpoint используется в связке с acceptor, то endpoint хранит адреса и порт откуда ждать соединения.

ProxyServer::ProxyServer(boost::asio::io_service &ioService,
						 const std::string &logFileName,
						 const std::string &serverHost,
						 uint16_t serverPort,
						 const std::string &dbHost,
						 uint16_t dbPort)
		: _ioService(ioService),
		logFileName(logFileName),
		_serverHost(serverHost),
		_serverPort(serverPort),
		_dbHost(dbHost),
		_dbPort(dbPort),
		_selfAddress(boost::asio::ip::address_v4::from_string(_serverHost)),
		_acceptor(_ioService,boost::asio::ip::tcp::endpoint(_selfAddress, _serverPort)) {}

bool ProxyServer::acceptConnection()
{
	std::cout << "accept connection!" << std::endl;
	try
	{
		_connector = boost::shared_ptr<Connector>(new Connector(_ioService));
		_acceptor.async_accept(
				_connector->getDatabaseToClientSocket(),
			boost::bind(&ProxyServer::accept_handler,
			   this,
			   boost::asio::placeholders::error));

	}catch (std::exception &exception){
		std::cerr << "error accept!" << std::endl;
		return false;
	}
	return true;
}

void ProxyServer::accept_handler(const boost::system::error_code &error)
{
	if (error)
		std::cerr << "Error! accept" << std::endl;
	else {
		_connector->start(_dbHost, _dbPort);

		if (! acceptConnection()){
			std::cerr << "Error! accept" << std::endl;
		}
	}
}
