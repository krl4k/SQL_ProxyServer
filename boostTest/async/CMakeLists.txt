cmake_minimum_required(VERSION 3.19)
project(sql_boost_server)
set(EXECUTABLE_NAME "sql_tcp_server")



add_executable(client
        client.cpp
        )


add_executable(server
        server.cpp
        )
