//
// Created by kirill on 03.05.2021.
//

#include "Connector.h"


Connector::Connector(boost::asio::io_service &ioService) :
		_downstram_socket(ioService), _upstream_socket(ioService)
{
}



void Connector::start(const std::string &DBhost, uint16_t DBport)
{
//	boost::asio::ip::tcp::endpoint ep( boost::asio::ip::address::from_string(_dbHost), DBport);
	std::cout << "start!" << std::endl;
	//_dbHost error!!!
	_upstream_socket.async_connect(
			boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(DBhost), DBport),
			boost::bind(&Connector::handle_connect,
			   shared_from_this(),
			   boost::asio::placeholders::error));
}

void Connector::handle_connect(const Connector::error_code &errorCode)
{
	std::cout << "handle connect!" << std::endl;
	if (!errorCode){
		_downstram_socket.async_read_some(
				boost::asio::buffer(_downstreamData, BUFSIZE),
				boost::bind(&Connector::sendData_toDB,
							shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred()));


		_upstream_socket.async_read_some(
				boost::asio::buffer(_upstreamData, BUFSIZE),
				boost::bind(&Connector::sendData_toClient,
							shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred()));

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

