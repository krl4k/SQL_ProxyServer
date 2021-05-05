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

enum COM {
	COM_SLEEP = 0,
	COM_QUIT = 1,
	COM_QUERY = 3,
	COM_CREATE_DB = 5,
	COM_DROP_DB = 6,
	COM_STATISTICS = 9
};


//Прокси-сервер TCP действует как посредник,
//чтобы «пересылать» TCP-соединения от внешних клиентов
//на отдельный удаленный сервер.
//Коммуникационный поток в направлении
//от клиента к прокси-серверу называется восходящим потоком ,
//а коммуникационный поток в направлении
//от сервера к прокси-серверу к клиенту называется нисходящим потоком .
class Connector : public boost::enable_shared_from_this<Connector>
{
public:

	using tcp_socket = boost::asio::ip::tcp::socket;
	using self_type_ptr = boost::shared_ptr<Connector>;
	typedef boost::system::error_code error_code;


	Connector(boost::asio::io_service &ioService, int logFileFd);


	void start(const std::string& DBhost, uint16_t DBport);

	void handle_connect(const error_code &errorCode);

	tcp_socket &getClientToDatabaseSocket();

	tcp_socket &getDatabaseToClientSocket() ;

	void closeConnection();


private:
	tcp_socket _upstream_socket;
	tcp_socket _downstram_socket;
	unsigned char _upstreamData[BUFSIZE];
	unsigned char _downstreamData[BUFSIZE];
	size_t _bytesCount;
	boost::mutex _mutex;

	int _logFileFd;


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

	void simpleLogger(const size_t &bytes);

	std::string getData();

	std::string getProtocol(unsigned char i);

	int getPayloadLength();
};


#endif //SQL_TCP_SERVER_CONNECTOR_H
