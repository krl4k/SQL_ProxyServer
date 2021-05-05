cmake_minimum_required(VERSION 3.19)
project(sql_tcp_server_boost)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-pthread")

set(SURS  ${CMAKE_CURRENT_SOURCE_DIR}/srcs/)


add_executable(proxy_server_boost
        ${SURS}Connector.cpp
        ${SURS}Connector_data_from_DB_toClient.cpp
        ${SURS}Connector_data_from_Client_to_DB.cpp
        ${SURS}ProxyServer.cpp
        ${SURS}main.cpp
        )

target_link_libraries(proxy_server_boost "-lboost_locale")