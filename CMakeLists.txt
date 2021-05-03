cmake_minimum_required(VERSION 3.19)
project(sql_tcp_server)
set(EXECUTABLE_NAME "sql_tcp_server")

set(CMAKE_CXX_STANDARD 14)
#set(SURS  ${CMAKE_CURRENT_SOURCE_DIR}/srcs/)
#
#set(SURS_CLASSES ${SURS}Server/ ${SURS}/Client)
#
#include_directories(./ ${SURS} ${SURS_CLASSES})
#link_directories(./ ${SURS} ${SURS_CLASSES})

#
#add_executable(${EXECUTABLE_NAME}
#        ${SURS}main.cpp
#        ${SURS}Server/Server.cpp
#        ${SURS}Client/Client.cpp
##        test.cpp
#        )

#set(CMAKE_CXX_FLAGS "-pthread")
#set(CMAKE_CXX_FLAGS "-lpthread")
#set(CMAKE_CXX_FLAGS "-lboost_system -lboost_thread")
#set(CMAKE_CXX_FLAGS "-D_GLIBCXX_USE_NANOSLEEP")
#set(CMAKE_CXX_FLAGS "-lboost_system -lboost_thread -lm  -lc")

set(CMAKE_CXX_FLAGS "-pthread")


#add_executable(boost_test
##        boostTest/main.cpp
#        boostTest/tcpClient.cpp
#        )
#
#add_executable(server
#        boostTest/tcpServer.cpp
#        )
#add_executable(client
#        boostTest/tcpClient.cpp
#        )

add_executable(ProxyServer
        srcBoostProxyServer/ProxyServer.cpp
        srcBoostProxyServer/Connector.cpp
        srcBoostProxyServer/Connector_data_from_Client_to_DB.cpp
        srcBoostProxyServer/Connector_data_from_DB_toClient.cpp
        srcBoostProxyServer/main.cpp
        )

#target_link_libraries("-lm")