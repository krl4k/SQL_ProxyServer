//
// Created by kirill on 03.05.2021.
//

#ifndef SQL_BOOST_SERVER_PROXYSERVER_H
#define SQL_BOOST_SERVER_PROXYSERVER_H


class ProxyServer : : public boost::enable_shared_from_this<ProxyServer>, boost::noncopyable
{
public:
	typedef boost::shared_ptr<ProxyServer> ptr;

private:
};


#endif //SQL_BOOST_SERVER_PROXYSERVER_H
