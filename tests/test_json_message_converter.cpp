//
// Created by max on 29.10.2022.
//

#include <gtest/gtest.h>

#include "common/json_message_converter.h"


TEST(TestJsonIncrementalMessageConverter, test_encode_decode_greeting)
{
    JsonIncrementalMessageConverter converter;
    GreetingMessage msg{.client_id="test_client", .tokens_count=42};

    auto bytes = converter.encodeGreeting(msg);

    ASSERT_TRUE(converter.processBytes(bytes.data(), bytes.size()));
    MessageVariant new_message = converter.getMessage();
    ASSERT_TRUE(std::holds_alternative<GreetingMessage>(new_message));

    MessageVariant unexpected_message = converter.getMessage();
    ASSERT_TRUE(std::holds_alternative<std::monostate>(unexpected_message));

    ASSERT_EQ(std::get<GreetingMessage>(new_message).client_id, "test_client");
    ASSERT_EQ(std::get<GreetingMessage>(new_message).tokens_count, 42);
}


TEST(TestJsonIncrementalMessageConverter, test_encode_decode_ready)
{
    JsonIncrementalMessageConverter converter;
    auto bytes = converter.encodeReady(ReadyMessage{});

    ASSERT_TRUE(converter.processBytes(bytes.data(), bytes.size()));
    MessageVariant new_message = converter.getMessage();
    ASSERT_TRUE(std::holds_alternative<ReadyMessage>(new_message));

    MessageVariant unexpected_message = converter.getMessage();
    ASSERT_TRUE(std::holds_alternative<std::monostate>(unexpected_message));
}

TEST(TestJsonIncrementalMessageConverter, test_encode_decode_token)
{
    JsonIncrementalMessageConverter converter;
    TokenMessage msg{.token="test_token"};

    auto bytes = converter.encodeToken(msg);

    ASSERT_TRUE(converter.processBytes(bytes.data(), bytes.size()));
    MessageVariant new_message = converter.getMessage();
    ASSERT_TRUE(std::holds_alternative<TokenMessage>(new_message));

    MessageVariant unexpected_message = converter.getMessage();
    ASSERT_TRUE(std::holds_alternative<std::monostate>(unexpected_message));

    ASSERT_EQ(std::get<TokenMessage>(new_message).token, "test_token");
}


TEST(TestJsonIncrementalMessageConverter, test_split_json_decode)
{
    // Test split JSON parsing
    JsonIncrementalMessageConverter converter;
    GreetingMessage msg{.client_id="test_client", .tokens_count=42};

    auto bytes = converter.encodeGreeting(msg);

    std::vector<char> part1(bytes.begin(), bytes.begin() + 5);
    std::vector<char> part2(bytes.begin() + 5, bytes.begin() + 10);
    std::vector<char> part3(bytes.begin() + 10, bytes.end());

    ASSERT_FALSE(converter.processBytes(part1.data(), part1.size()));
    ASSERT_FALSE(converter.processBytes(part2.data(), part2.size()));
    ASSERT_TRUE(converter.processBytes(part3.data(), part3.size()));

    MessageVariant new_message = converter.getMessage();
    ASSERT_TRUE(std::holds_alternative<GreetingMessage>(new_message));

    MessageVariant unexpected_message = converter.getMessage();
    ASSERT_TRUE(std::holds_alternative<std::monostate>(unexpected_message));
}

TEST(TestJsonIncrementalMessageConverter, test_multiple_json_decode)
{
    // Test multiple-per-once JSON parsing
    JsonIncrementalMessageConverter converter;

    ReadyMessage msg1{};
    TokenMessage msg2{.token="test_token"};

    auto bytes = converter.encodeReady(msg1);
    auto bytes_msg2 = converter.encodeToken(msg2);
    bytes.insert(bytes.end(), bytes_msg2.begin(), bytes_msg2.end());

    ASSERT_TRUE(converter.processBytes(bytes.data(), bytes.size()));

    MessageVariant new_message1 = converter.getMessage();
    ASSERT_TRUE(std::holds_alternative<ReadyMessage>(new_message1));

    MessageVariant new_message2 = converter.getMessage();
    ASSERT_TRUE(std::holds_alternative<TokenMessage>(new_message2));

    MessageVariant unexpected_message = converter.getMessage();
    ASSERT_TRUE(std::holds_alternative<std::monostate>(unexpected_message));
}
