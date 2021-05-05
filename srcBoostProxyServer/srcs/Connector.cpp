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
	_upstream_socket.async_connect(
			boost::asio::ip::tcp::endpoint(
					boost::asio::ip::address::from_string(DBhost), DBport),
				boost::bind(&Connector::handle_connect,
			   	shared_from_this(),
			   		boost::asio::placeholders::error));
}

void Connector::handle_connect(const Connector::error_code &errorCode)
{
	if (!errorCode){

		//read from Db
		_upstream_socket.async_read_some(
				boost::asio::buffer(_upstreamData, BUFSIZE),
				boost::bind(&Connector::sendData_toClient,
							shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));


		//read from client
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
	std::string date = getData();
	write(_logFileFd, "\n-----------------------------------------\n", 43);
	write(_logFileFd, "Data: ", 5);
	write(_logFileFd, date.c_str(), date.size());
	write(_logFileFd, "\n", 1);
	write(_logFileFd, "Text protocol: ", 15);

	for (int i = 0; i < 5; ++i)
		std::cout << std::hex << unsigned (_downstreamData[i]) << " ";
	std::cout  << std::endl;


	if (_downstreamData[4] == 3)
		write(_logFileFd, "COM_QUERY\n", 10);

	for (size_t i = 5; i < bytes - 5; ++i){
		std::cout << (_downstreamData[i]);
		write(_logFileFd, "\n", 1);
	}

	write(_logFileFd, "\n-----------------------------------------\n", 43);
}

std::string Connector::getData()
{
	std::string date;
	time_t rawtime;
	time(&rawtime);
	date = ctime(&rawtime);
	date.erase(date.size() - 1);
	return date;
}


//	std::ios init(NULL);
//	init.copyfmt(std::cout);
//	std::cout.copyfmt(init);
//	std::cout  << std::endl;

