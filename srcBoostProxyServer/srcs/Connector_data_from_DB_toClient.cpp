//

// Created by kirill on 03.05.2021.
//

#include "Connector.h"

void Connector::sendData_toClient(const boost::system::error_code &error,
								  const size_t &bytes)
{
	if (error)
		closeConnection();
	else {
		boost::asio::async_write(
				_downstram_socket,
				boost::asio::buffer(_upstreamData, bytes),
				boost::bind(&Connector::readData_fromDB,
							shared_from_this(),
							boost::asio::placeholders::error));

	}
}

void Connector::readData_fromDB(const boost::system::error_code &error)
{

	if (!error){
		_upstream_socket.async_read_some(
				boost::asio::buffer(_upstreamData, BUFSIZE),
				boost::bind(&Connector::sendData_toClient,
							shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
	}else
		closeConnection();
}


