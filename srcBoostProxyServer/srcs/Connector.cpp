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


/*!
 * @param count bytes
 * @warning doesnt correctly work with SSL enabled
 * @details
 * Protocol::Packet (https://dev.mysql.com/doc/internals/en/mysql-packet.html) <br/>
 * int<3> - payload length <br/>
 * int<1> - sequence id <br/>
 * string - payload (Command = 1, Statement = payload length - 1) <br/>
 */
void Connector::simpleLogger(const size_t &bytes)
{
	if (getProtocol(_downstreamData[4]) != "") {
		write(_logFileFd, "\n-----------------------------------------\n", 43);

		std::string date = "Data: " + getData() + "\n";
		write(_logFileFd, date.c_str(), date.size());

		int payloadLength = getPayloadLength();
		std::string pL = "Payload length: " + std::to_string(payloadLength) + "\n";
		write(_logFileFd, pL.c_str(), pL.size());

		std::string packetNumber = "Packet number: " + std::to_string((int)_downstreamData[3]) + "\n";
		write(_logFileFd, packetNumber.c_str(), packetNumber.size());


		std::string textProtocol = "Text protocol: " + getProtocol(_downstreamData[4]);
		write(_logFileFd, textProtocol.c_str(), textProtocol.size());

		write(_logFileFd, "Payload:\n", 9);
		write(_logFileFd, &_downstreamData[5], payloadLength - 1); // - 1 because _data[4] - is Command

		write(_logFileFd, "\n-----------------------------------------\n", 43);
	}
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

/*!
 *	@brief like Java enum.name()
 */
std::string Connector::getProtocol(unsigned char i)
{
	if (i == COM_SLEEP)
		return "COM_SLEEP\n";
	else if (i == COM_QUIT)
		return "COM_QUIT\n";
	else if (i == COM_QUERY)
		return "COM_QUERY\n";
	else if (i == COM_CREATE_DB)
		return "COM_CREATE_DB\n";
	else if (i == COM_DROP_DB)
		return "COM_DROP_DB\n";
	else if (i == COM_STATISTICS)
		return "COM_STATISTICS\n";
	return "";
}

int Connector::getPayloadLength()
{
	return  int(int(_downstreamData[0]) |
				int(_downstreamData[1]) << 8 |
				int(_downstreamData[2]) << 16);
}


//	std::ios init(NULL);
//	init.copyfmt(std::cout);
//	std::cout.copyfmt(init);
//	std::cout  << std::endl;

