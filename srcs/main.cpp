#include <iostream>
#include <unistd.h>
#include "Server.hpp"
#include <csignal>

void error_server(int sig){
	exit(0);
}

int main(int argc, char **argv) {
	if (argc == 2) {
		signal(SIGTERM, error_server);
		signal(SIGINT, error_server);
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

//    struct sockaddr_in addr;
//    int servSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TTP);
//
//    memset(&addr, 0, sizeof(addr));
//    addr.sin_family = AF_INET;
//    addr.sin_addr.s_addr = htonl(INADDR_ANY);
//    addr.sin_port = htons(5432);
//
//    bind(servSocket, (struct sockaddr*)&addr, sizeof addr);
//    listen(servSocket, 10);
//    int connectFd;
//    char buf[1234];
////    while (1){
//        connectFd = accept(servSocket, (struct sockaddr*)NULL, NULL);
//
//        int rb = recv(connectFd, buf, 1234, 0);
//        buf[rb] = 0;
//        std::cout << "buf = " << buf << std::endl;
//    }

//    struct sockaddr_in addr;
//    bzero(&addr, sizeof(addr));
//    addr.sin_family = AF_INET;
//    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//    addr.sin_port = htons(5432);
//
//
//
//    int socketDB = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
//    if (connect(socketDB, (struct sockaddr*)&addr, sizeof(addr))) {
//        std::cout << "error connect!" << std::endl;
//    }
//
//    std::string sql_request = "SELECT * FROM person";
//
//    send(socketDB, sql_request.c_str(), sql_request.size(), 0);
//
//    char buf[1000];
//    int reb = recv(socketDB, buf, 1000, 0);
//    buf[reb] = 0;
//
//    std::cout << "buf:" << buf << std::endl;

	return 0;
}
/*


int main(int argc, char *argv[]) {
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;

    memset(recvBuff, '0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(5432);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\n inet_pton error occured\n");
        return 1;
    }
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n Error : Connect Failed \n");
        return 1;
    }
    std::cout << "block read" << std::endl;
    read(sockfd, recvBuff, 7);
//    while ((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0) {
//        recvBuff[n] = 0;
//        if (fputs(recvBuff, stdout) == EOF) {
//            printf("\n Error : Fputs error\n");
//        }
//    }

    std::cout << "recv:" << recvBuff << std::endl;
    if (n < 0) {
        printf("\n Read error \n");
    }

    return 0;
}*/
