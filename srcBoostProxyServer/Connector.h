//
// Created by kirill on 03.05.2021.
//

#ifndef SQL_TCP_SERVER_CONNECTOR_H
#define SQL_TCP_SERVER_CONNECTOR_H


#define BUFSIZE 2048
#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>


class Connector : public boost::enable_shared_from_this<Connector>, boost::noncopyable
{
public:

//	using namespace ip = boost::asio::ip;
	using tcp_socket = boost::asio::ip::tcp::socket;
	using self_type_ptr = boost::shared_ptr<Connector>;
	typedef boost::system::error_code error_code;


	Connector(boost::asio::io_service &ioService);


	void start(const std::string& DBhost, uint16_t DBport);

	void handle_connect(const error_code &errorCode);

	const tcp_socket &getDatabaseSocket() const;

	const tcp_socket &getClientSocket() const;

	void closeConnection();


private:
	tcp_socket _client_to_database_Socket;
	tcp_socket _database_to_client_Socket;
	char *_data_from_Client_toDB[BUFSIZE];
	char *_data_fromDB_toClient[BUFSIZE];
	boost::mutex _mutex;
	std::string _dbHost;
	std::string _dbPort;

	void sendData_toClient(const boost::system::error_code& error,
							  const size_t& bytes);


	void readData_fromDB(const boost::system::error_code& error);

};


#endif //SQL_TCP_SERVER_CONNECTOR_H
