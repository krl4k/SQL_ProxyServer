//
// Created by kirill on 03.05.2021.
//

#ifndef SQL_BOOST_SERVER_PROXYSERVER_H
#define SQL_BOOST_SERVER_PROXYSERVER_H

#include <fstream>


#define BUFSIZE 2048

#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>

class ProxyServer
{
public:

	ProxyServer(boost::asio::io_service &ioService, const std::string &logFileName,
				const std::string &serverHost, uint16_t serverPort, const std::string &dbHost,
				uint16_t dbPort);



private:

	boost::asio::io_service& _ioService;
	std::string logFileName;
	std::string _serverHost;
	uint16_t _serverPort;
	std::string _dbHost;
	uint16_t 	_dbPort;
	boost::asio::ip::address _selfAddress;

	boost::asio::ip::tcp::acceptor _acceptor;

	int _logFileFd;

};


#endif //SQL_BOOST_SERVER_PROXYSERVER_H
