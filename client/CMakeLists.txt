project(mkt_conn_client)

set(CMAKE_CXX_STANDARD 17)


set(SOURCES
        ../common/json_message_converter.cpp
        src/tcp_client.cpp)

find_package(Boost REQUIRED COMPONENTS program_options json)
find_package(spdlog REQUIRED)

add_library(mkt_conn_client_lib ${SOURCES})
target_include_directories(mkt_conn_client_lib PRIVATE src)
target_include_directories(mkt_conn_client_lib PRIVATE ..)

add_executable(mkt_conn_client src/main.cpp)
target_include_directories(mkt_conn_client PRIVATE src)
target_include_directories(mkt_conn_client PRIVATE ..)
target_link_libraries(mkt_conn_client
        mkt_conn_client_lib
        Boost::program_options
        Boost::json
        spdlog::spdlog
        )
