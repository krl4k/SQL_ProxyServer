#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
//#include <boost/asio/detail/thread_group.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/detail/posix_signal_blocker.hpp>
//#include <boost/phoenix/bind/bind_member_variable.hpp>
//#include <boost/timer/timer.hpp>

//#include <thread.h>
//#include <threads.h>
//#include <boost/asio/impl/src.hpp>

using namespace boost::asio;

boost::asio::io_service service;

ip::tcp::endpoint ep( ip::address::from_string("127.0.0.1"), 8001);
//int _1;
//int _2;



size_t read_complete(char * buf, const std::error_code & err, size_t bytes)
{
	if ( err) return 0;
	bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
	// we read one-by-one until we get to enter, no buffering
	return found ? 0 : 1;
}
void sync_echo(std::string msg)
{
	msg += "\n";
	ip::tcp::socket sock(service);
	sock.connect(ep);
	sock.write_some(buffer(msg));
	char buf[1024];
	int bytes = read(sock, buffer(buf), boost::bind(read_complete, buf, _1, _2));
	std::string copy(buf, bytes - 1);
	msg = msg.substr(0, msg.size() - 1);
	std::cout << "server echoed our " << msg << ": "<< (copy == msg ? "OK" : "FAIL") << std::endl;
	sock.close();
}
int main(int argc, char* argv[])
{
	char* messages[] = { "John says hi", "so does James", "Lucy just got home", "Boost.Asio is Fun!", 0 };
	boost::asio::detail::thread_group threads;
	for ( char ** message = messages; *message; ++message)
	{
		threads.create_thread(boost::bind(sync_echo, *message));
//		std::this_thread::sleep(boost::posix_time::millisec(100));
//		std::this_thread::sleep_for(boost::posix_time::millisec(100));
		usleep(100);
	}
	threads.join();
}