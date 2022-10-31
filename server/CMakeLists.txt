project(mkt_conn_server)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS program_options json)
find_package(spdlog REQUIRED)

set(SOURCES
        ../common/json_message_converter.cpp
        src/client_connection.cpp
        src/tcp_server.cpp
        src/consumer.cpp
        src/consumer_manager.cpp)

add_library(mkt_conn_server_lib ${SOURCES})

target_include_directories(mkt_conn_server_lib PRIVATE ${CONAN_INCLUDE_DIRS})
target_include_directories(mkt_conn_server_lib PRIVATE src)
target_include_directories(mkt_conn_server_lib PRIVATE ..)

add_executable(mkt_conn_server src/main.cpp)
target_include_directories(mkt_conn_server PRIVATE src)
target_include_directories(mkt_conn_server PRIVATE ../)
target_include_directories(mkt_conn_server PRIVATE ${CONAN_INCLUDE_DIRS})
target_link_libraries(mkt_conn_server
        mkt_conn_server_lib
        Boost::program_options
        Boost::json
        spdlog::spdlog
        )
