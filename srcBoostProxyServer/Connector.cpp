//
// Created by kirill on 03.05.2021.
//

#include "Connector.h"


Connector::Connector(boost::asio::io_service &ioService, int logFileFd) :
		_downstram_socket(ioService), _upstream_socket(ioService), _mutex(),
		_bytesCount(0), _logFileFd(logFileFd)
{
}



void Connector::start(const std::string &DBhost, uint16_t DBport)
{
	std::cout << "start!" << std::endl;
	//_dbHost error!!!
	_upstream_socket.async_connect(
			boost::asio::ip::tcp::endpoint(
					boost::asio::ip::address::from_string(DBhost), DBport),
				boost::bind(&Connector::handle_connect,
			   	shared_from_this(),
			   		boost::asio::placeholders::error));
}

void Connector::handle_connect(const Connector::error_code &errorCode)
{
	std::cout << "handle connect!" << std::endl;
	if (!errorCode){
		_upstream_socket.async_read_some(
				boost::asio::buffer(_upstreamData, BUFSIZE),
				boost::bind(&Connector::sendData_toClient,
							shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));


		_downstram_socket.async_read_some(
				boost::asio::buffer(_downstreamData, BUFSIZE),
				boost::bind(&Connector::sendData_toDB,
							shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));

	}else
		closeConnection();

}


void Connector::closeConnection()
{
	boost::mutex::scoped_lock lock(_mutex);
	if (_downstram_socket.is_open())
		_downstram_socket.close();
	if (_upstream_socket.is_open())
		_upstream_socket.close();
}

Connector::tcp_socket &Connector::getClientToDatabaseSocket()
{
	return _upstream_socket;
}

Connector::tcp_socket &Connector::getDatabaseToClientSocket()
{
	return _downstram_socket;
}

void Connector::simpleLogger(const size_t &bytes)
{
	std::string date;
	time_t rawtime;
	time(&rawtime);
	date = ctime(&rawtime);
	date.erase(date.size() - 1);
	write(_logFileFd, "\n-----------------------------------------\n", 43);
	write(_logFileFd, "Data: ", 5);
	write(_logFileFd, date.c_str(), date.size());
	write(_logFileFd, "\n", 1);
	for (int i = 5; i < bytes - 1; ++i){
		write(_logFileFd, _downstreamData, 1);
	}
	write(_logFileFd, "\n-----------------------------------------\n", 43);

}

