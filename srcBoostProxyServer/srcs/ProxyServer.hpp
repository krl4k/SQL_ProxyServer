//
// Created by kirill on 03.05.2021.
//

#ifndef SQL_BOOST_SERVER_PROXYSERVER_H
#define SQL_BOOST_SERVER_PROXYSERVER_H

#include <fstream>


#include "Connector.h"
//
//#include <iostream>
//#include <boost/asio/ip/tcp.hpp>
//#include <boost/asio.hpp>
//#include <boost/bind.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/enable_shared_from_this.hpp>
//#include <boost/noncopyable.hpp>
//#include <boost/thread/mutex.hpp>

class ProxyServer
{
public:

	ProxyServer(boost::asio::io_service &ioService, const std::string &logFileName,
				const std::string &serverHost, uint16_t serverPort, const std::string &dbHost,
				uint16_t dbPort);

	bool acceptConnection();

	virtual ~ProxyServer();

private:

	void accept_handler(const boost::system::error_code &error);

	boost::asio::io_service& _ioService;
	std::string logFileName;
	std::string _serverHost;
	uint16_t _serverPort;
	std::string _dbHost;
	uint16_t 	_dbPort;
	boost::asio::ip::address_v4 _selfAddress;

	boost::asio::ip::tcp::acceptor _acceptor;
	boost::shared_ptr<Connector> _connector;

	int _logFileFd;

};


#endif //SQL_BOOST_SERVER_PROXYSERVER_H
