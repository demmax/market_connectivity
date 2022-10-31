//
// Created by max on 29.10.2022.
//

#pragma once

#include <string>
#include <variant>


struct GreetingMessage
{
    std::string client_id;
    size_t tokens_count;
};

struct ReadyMessage
{
};

struct TokenMessage
{
    std::string token;
};

using MessageVariant = std::variant<std::monostate,
                                    GreetingMessage,
                                    ReadyMessage,
                                    TokenMessage>;
