//
// Created by kirill on 03.05.2021.
//

#include "Connector.h"


void Connector::sendData_toDB(const boost::system::error_code &error, const size_t &bytes)
{
	std::cout << "send data to db!" << std::endl;
	if (!error){
		// error bytes (BUFSIZE!!!!! shit)
		_bytesCount = bytes;
		boost::asio::async_write(
				_upstream_socket,
				boost::asio::buffer(_downstreamData, bytes),
				boost::bind(&Connector::readData_fromClient,
				shared_from_this(),
				boost::asio::placeholders::error));

		simpleLogger(bytes);
		std::cout << std::endl;
	}else
		closeConnection();
}

void Connector::readData_fromClient(const boost::system::error_code &error)
{
	std::cout << "read data from client!" << std::endl;
	if (error)
		closeConnection();
	else{
		_downstram_socket.async_read_some(
				boost::asio::buffer(_downstreamData, BUFSIZE),
				boost::bind(&Connector::sendData_toDB,
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
}