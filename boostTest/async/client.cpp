//
// Created by kirill on 03.05.2021.
//
#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/bind.hpp>
//#include <boost/asio/detail/thread_group.hpp>
//#include <boost/asio/thread_pool.hpp>
//#include <boost/asio/detail/posix_signal_blocker.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>



#define MEM_FN(x) boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x, y) boost::bind(&self_type::x, shared_from_this(),y)
#define MEM_FN2(x, y, z) boost::bind(&self_type::x, shared_from_this(),y,z)

using namespace boost::asio;

//io_service – самый важный класс в библиотеке,
// он имеет дело с операционной системой,
// ждет конца всех асинхронных операций,
// а потом при завершении вызывает обработчик для каждой такой операции.
boost::asio::io_service service;


class talk_to_svr : public boost::enable_shared_from_this<talk_to_svr>, boost::noncopyable
{
	typedef talk_to_svr self_type;

	talk_to_svr(const std::string &message) : sock_(service), started_(true), message_(message)
	{}

	void start(ip::tcp::endpoint ep)
	{
		sock_.async_connect(ep, MEM_FN1(on_connect, _1));
	}

public:
	typedef boost::system::error_code error_code;
	typedef boost::shared_ptr <talk_to_svr> ptr;

	static ptr start(ip::tcp::endpoint ep, const std::string &message)
	{
		ptr new_(new talk_to_svr(message));
		new_->start(ep);
		return new_;
	}

	void stop()
	{
		if(!started_)
			return;
		started_ = false;
		sock_.close();
	}

	bool started()
	{ return started_; }
	//	...


	void do_read()
	{
		async_read(sock_, buffer(read_buffer_), MEM_FN2(read_complete, _1, _2), MEM_FN2(on_read, _1, _2));
	}

	void do_write(const std::string &msg)
	{
		if(!started())
			return;
		std::copy(msg.begin(), msg.end(), write_buffer_);
		sock_.async_write_some(buffer(write_buffer_, msg.size()), MEM_FN2(on_write, _1, _2));
	}

	size_t read_complete(const std::error_code & err, size_t bytes)
	{
		if ( err) return 0;
		bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
		std::cout << "buffer: " << std::endl;
		for (size_t i = 0; i < bytes; ++i)
		{
			std::cout << read_buffer_[i];
		}
		std::cout  << std::endl;
		// we read one-by-one until we get to enter, no buffering
		return found ? 0 : 100;
	}

	void on_connect(const error_code & err)
	{
		if (!err)
			do_write(message_ + "\n");
		else stop();
	}
	void on_read(const error_code & err, size_t bytes)
	{
		if ( !err)
		{
			std::string copy(read_buffer_, bytes - 1);
			std::cout << "server echoed our " << message_ << ": "<< (copy == message_ ? "OK" : "FAIL") << std::endl;
		}
		stop();
	}
	void on_write(const error_code & err, size_t bytes)
	{
		do_read();
	}

private:
	ip::tcp::socket sock_;
	enum
	{
		max_msg = 1024
	};
	char read_buffer_[max_msg];
	char write_buffer_[max_msg];
	bool started_;
	std::string message_;
};

int main(int argc, char* argv[])
{
	ip::tcp::endpoint ep( ip::address::from_string("127.0.0.1"), 8001);
	char* messages[] = { "John says hi", "so does James", "Lucy got home", 0 };
	for ( char ** message = messages; *message; ++message)
	{
		talk_to_svr::start( ep, *message);
//		boost::this_thread::sleep( boost::posix_time::millisec(100));
		usleep(100);
	}
	std::cout << "before run()" << std::endl;
	service.run();
	std::cout << "after run()" << std::endl;
}
