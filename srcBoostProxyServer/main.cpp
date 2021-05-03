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

std::string &getConfigArgs(const std::string &fileName)
{
	std::ifstream file(fileName);
	if(!file.is_open())
		throw std::runtime_error("File not found!!!");

	std::string strings[5];
	for (int i = 0; i < 5; ++i)
		std::getline(file, strings[i]);
	file.close();
	if("proxy_host: " == strings[PROXY_HOST].substr(0, 12))
	{
		_serverHost = std::string(strings[PROXY_HOST], 12);
	}
	if("proxy_port: " == strings[PROXY_PORT].substr(0, 12))
	{
		std::string port = strings[PROXY_PORT].substr(12);
		if((_serverPort = convertPort(port)) < 0)
			throw std::exception();
	}
	if("database_host: " == strings[DB_HOST].substr(0, 15))
	{
		_dbHost = std::string(strings[DB_HOST], 15);
	}
	if("database_port: " == strings[DB_PORT].substr(0, 15))
	{
		std::string port = strings[DB_PORT].substr(15);
		if((_dbPort = convertPort(port)) < 0)
			throw std::exception();
	}
	if("log_file: " == strings[LOG_FILE_NAME].substr(0, 10))
	{
		std::string logFile(strings[LOG_FILE_NAME], 10);
		if((_logFileFd = open(logFile.c_str(), O_WRONLY | O_TRUNC)) < 0)
		{
			throw std::runtime_error("logFile error not opened!!!");
		}
	}
	for (std::string str : strings){
		if (str.empty())
			throw std::runtime_error("parse error");
	}
	file.close();
}

int main(int argc, char **argv)
{
	boost::asio::io_service ioService;

	if(argc == 2)
	{
		try
		{
			std::string *config = getConfigArgs(argv[1]);
			ProxyServer proxyServer(ioService, config[LOG_FILE_NAME],
						   config[PROXY_HOST], config[PROXY_PORT],
						   config[DB_HOST], config[DB_PORT]);

			proxyServer.
		} catch (std::exception &exception)
		{
			std::cout << exception.what() << std::endl;
		}
	}
	else
		std::cerr << "Wrong argument! Give config file!!!" << std::endl;
	return (0);
}
