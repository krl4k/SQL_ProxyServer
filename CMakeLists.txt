cmake_minimum_required(VERSION 3.19)
project(sql_tcp_server)
set(EXECUTABLE_NAME "sql_tcp_server")

set(CMAKE_CXX_STANDARD 14)
set(SURS  ${CMAKE_CURRENT_SOURCE_DIR}/srcs/)

set(SURS_CLASSES ${SURS}Server/ ${SURS}/Client)

include_directories(./ ${SURS} ${SURS_CLASSES})
link_directories(./ ${SURS} ${SURS_CLASSES})

#
#add_executable(${EXECUTABLE_NAME}
#        ${SURS}main.cpp
#        ${SURS}Server/Server.cpp
#        ${SURS}Client/Client.cpp
##        test.cpp
#        )

set(CMAKE_CXX_FLAGS "-pthread")

add_executable(boost_test
        boostTest/main.cpp)