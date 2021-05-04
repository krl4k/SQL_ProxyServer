//
// Created by kirill on 03.05.2021.
//

#ifndef SQL_TCP_SERVER_CONNECTOR_H
#define SQL_TCP_SERVER_CONNECTOR_H


#define BUFSIZE 10009
#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>


class Connector : public boost::enable_shared_from_this<Connector>
{
public:

//	using namespace ip = boost::asio::ip;
	using tcp_socket = boost::asio::ip::tcp::socket;
	using self_type_ptr = boost::shared_ptr<Connector>;
	typedef boost::system::error_code error_code;


	Connector(boost::asio::io_service &ioService);


	void start(const std::string& DBhost, uint16_t DBport);

	void handle_connect(const error_code &errorCode);

	tcp_socket &getClientToDatabaseSocket();

	tcp_socket &getDatabaseToClientSocket() ;

	void closeConnection();


private:
	tcp_socket _upstream_socket;
	tcp_socket _downstram_socket;
	char _upstreamData[BUFSIZE];
	char _downstreamData[BUFSIZE];
	size_t _bytesCount;
	boost::mutex _mutex;


	/*
	 *  database -> proxy -> client
	 */

	void sendData_toClient(const boost::system::error_code& error,
							  const size_t& bytes);

	void readData_fromDB(const boost::system::error_code& error);


	/*
	 *  client -> proxy -> database
	 */

	void sendData_toDB(const boost::system::error_code& error,
						   const size_t& bytes);


	void readData_fromClient(const boost::system::error_code& error);

};


#endif //SQL_TCP_SERVER_CONNECTOR_H
