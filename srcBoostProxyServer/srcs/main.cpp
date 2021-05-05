//
// Created by kirill on 03.05.2021.
//

#include <iostream>
#include <boost/asio/io_service.hpp>
#include "ProxyServer.hpp"

enum {
	PROXY_HOST,
	PROXY_PORT,
	DB_HOST,
	DB_PORT,
	LOG_FILE_NAME
};

struct Config{
	std::string proxy_host;
	std::string proxy_port;
	std::string db_host;
	std::string db_port;
	std::string log_file_name;
};


uint16_t convertPort(const std::string &port) {
	uint16_t p;
	try{
		p = static_cast<uint16_t>(std::stoi(port));
	}
	catch (std::exception &e){
		std::cout << "Port setting error" << std::endl;
		return -1;
	}
	return p;
}

void getConfigArgs(struct Config *config, const std::string &fileName)
{
	std::ifstream file(fileName);
	if(!file.is_open())
		throw std::runtime_error("File not found!!!");

	std::string strings[5];
	for (int i = 0; i < 5; ++i)
		std::getline(file, strings[i]);
	file.close();
	if("proxy_host: " == strings[PROXY_HOST].substr(0, 12))
		config->proxy_host = std::string(strings[PROXY_HOST], 12);
	if("proxy_port: " == strings[PROXY_PORT].substr(0, 12))
		config->proxy_port = strings[PROXY_PORT].substr(12);
	if("database_host: " == strings[DB_HOST].substr(0, 15))
		config->db_host = std::string(strings[DB_HOST], 15);
	if("database_port: " == strings[DB_PORT].substr(0, 15))
		config->db_port = strings[DB_PORT].substr(15);
	if("log_file: " == strings[LOG_FILE_NAME].substr(0, 10))
		config->log_file_name = strings[LOG_FILE_NAME].substr(10);
	file.close();
}

int main(int argc, char **argv)
{
	boost::asio::io_service ioService;

	if (argc == 2)
	{
		try
		{
			struct Config config;
			getConfigArgs(&config, argv[1]);

			ProxyServer proxyServer(ioService,
						   config.log_file_name,
						   config.proxy_host, convertPort(config.proxy_port),
						   config.db_host, convertPort(config.db_port));

			proxyServer.acceptConnection();
			std::cout << "server started!" << std::endl;
			ioService.run();
		} catch (std::exception &exception)
		{
			std::cout << "Server deleted!" << std::endl;
			std::cout << exception.what() << std::endl;
		}
	}
	else
		std::cerr << "Wrong argument! Give config file!!!" << std::endl;
	return (0);
}
