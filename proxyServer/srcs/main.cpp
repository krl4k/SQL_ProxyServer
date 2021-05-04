#include <iostream>
#include <unistd.h>
#include "Server.hpp"

int main(int argc, char **argv) {
	if (argc == 2) {
		try {
			Server server(argv[1]);
			server.start();
		} catch (std::exception &exception) {
			std::cerr << exception.what() << std::endl;
			std::cerr << "Server dont Started!!!" << std::endl;
		}
	} else {
		std::cerr << "Wrong argument!" << std::endl;
	}
	return 0;
}