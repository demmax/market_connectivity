project(mkt_conn_unit_tests)

enable_testing()
set(SOURCES
        ../common/json_message_converter.cpp
        ../server/src/consumer_manager.cpp
        ../server/src/consumer.cpp
        )

set(TEST_SOURCES
        test_json_message_converter.cpp
        test_consumer_manager.cpp)

find_package(spdlog REQUIRED)
find_package(GTest REQUIRED)
find_package(Boost REQUIRED COMPONENTS json)

add_executable(mkt_conn_unit_tests ${SOURCES} ${TEST_SOURCES})

target_link_libraries(mkt_conn_unit_tests
        GTest::GTest
        Boost::json
        spdlog::spdlog)

target_include_directories(mkt_conn_unit_tests PRIVATE ../)
target_include_directories(mkt_conn_unit_tests PRIVATE ../server/src)
