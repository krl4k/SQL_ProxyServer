#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
//#include <boost/asio/impl/src.hpp>

using namespace boost::asio;

//#define BOOST_ASIO_DISABLE_THREADS
//#define BOOST_ASIO_SEPARATE_COMPILATION
//#undef BOOST_ASIO_HEADER_ONLY

io_service service;



void read_write_tcp(){
	ip::tcp::endpoint ep( ip::address::from_string("127.0.0.1"), 80);
	ip::tcp::socket sock(service);
	sock.connect(ep);
	sock.write_some(buffer("GET /index.html\r\n"));
	std::cout << "bytes available " << sock.available() << std::endl;
	char buff[512];
	size_t read = sock.read_some(buffer(buff));
	std::cout << "buffer:\n" << buff << std::endl;
}

void test(){

	ip::tcp::endpoint ep( ip::address::from_string("127.0.0.1"), 80);
	ip::tcp::socket sock(service);
	sock.open(ip::tcp::v4());
	sock.connect(ep);
	sock.write_some(buffer("GET /index.html\r\n"));
	const int bs = 126;
	char buff[bs];
	sock.read_some(buffer(buff,bs));

	std::cout << "buffer:" << buff << std::endl;

	sock.shutdown(ip::tcp::socket::shutdown_receive);
	sock.close();
}

int main() {

//	test();
	read_write_tcp();
//	ip::address address = ip::address::from_string("127.0.0.1");
//	ip::tcp::endpoint ep(address, 8080);
//
//	ip::tcp::socket socket(service);
//	basic_stream_socket<> socket1(service);
//	socket.set_option(ip::tcp::socket::reuse_address(true));
//
//	std::cout << address.to_string() << std::endl;
//
//	ip::address_v4;
//	ip::tcp
}
