cmake_minimum_required(VERSION 3.19)
project(sql_tcp_server)

set(CMAKE_CXX_STANDARD 14)
set(SURS  ${CMAKE_CURRENT_SOURCE_DIR}/srcs/)
set(CMAKE_CXX_FLAGS "-pthread")


#link_directories(./ ${SURS}/Server ${SURS}/Client)
include_directories(./ ${SURS}/Server ${SURS}/Client)

add_executable(proxy_server
        ${SURS}main.cpp
        ${SURS}Server/Server.cpp
        ${SURS}Client/Client.cpp
        )

