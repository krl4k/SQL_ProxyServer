//
// Created by kirill on 03.05.2021.
//

#include "Connector.h"

void Connector::closeConnection()
{
	boost::mutex::scoped_lock lock(_mutex);
	_database_to_client_Socket.close();
	_client_to_database_Socket.close();
}

Connector::Connector(boost::asio::io_service &ioService) : _database_to_client_Socket(ioService), _client_to_database_Socket(ioService)
{
}

const Connector::tcp_socket &Connector::getDatabaseSocket() const
{
	return _client_to_database_Socket;
}

const Connector::tcp_socket &Connector::getClientSocket() const
{
	return _database_to_client_Socket;
}

void Connector::start(const std::string &DBhost, uint16_t DBport)
{
	boost::asio::ip::tcp::endpoint ep( boost::asio::ip::address::from_string(_dbHost), DBport);
	_client_to_database_Socket.async_connect(ep,
											 boost::bind(&Connector::handle_connect, shared_from_this(),
			 boost::asio::placeholders::error));
}

void Connector::handle_connect(const Connector::error_code &errorCode)
{
	if (errorCode){
		closeConnection();
	}else {
		//read from db server
		_client_to_database_Socket.async_read_some(
				boost::asio::buffer(_data_from_Client_toDB, BUFSIZE),
				boost::bind(&Connector::readFromClient_sendToDb,
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred()));

		//read from client
		_database_to_client_Socket.async_read_some(
				boost::asio::buffer(_data_fromDB_toClient, BUFSIZE),
				boost::bind(&Connector::readData_fromDB,
							shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred()));

	}
}

void Connector::sendData_toClient(const boost::system::error_code &error,
										const size_t &bytes)
{
	if (error)
		closeConnection();
	else {
		boost::asio::async_write(
				_database_to_client_Socket,
				boost::asio::buffer(_data_from_Client_toDB, bytes),
				boost::bind(&Connector::readData_fromDB,
				shared_from_this(),
				boost::asio::placeholder::error));

	}
}



void Connector::readData_fromDB(const boost::system::error_code &error)
{

}

