#include "tokenize.hpp"

#include <iostream>
#include <stdexcept>

namespace
{
    using QuoteTokens = std::vector<std::pair<bool, Token>>;

    QuoteTokens split_quote(std::string const &line)
    {
        QuoteTokens quote_tokens;
        std::string::size_type start = 0;
        std::string::size_type end = std::string::npos;
        auto in_quotes = false;
        auto delim = "\"";
        auto open_quote = false;

        auto add_if_not_empty = [&](Token const &token)
        {
            if (token.find_first_not_of(' ') != Token::npos)
            {
                quote_tokens.emplace_back(in_quotes, token);
            }
        };

        while ((end = line.find(delim, start)) != std::string::npos)
        {
            if (end != start)
            {
                add_if_not_empty(line.substr(start, end - start));
                in_quotes = !in_quotes;
            }
            else
            {
                in_quotes = true;
            }
            start = end + 1;
            open_quote = !open_quote;
        }
        if (open_quote)
        {
            throw std::runtime_error("Argument contains open quotes.");
        }
        if (start != line.size())
        {
            add_if_not_empty(line.substr(start));
        }
        return quote_tokens;
    }

    Tokens split(std::string const &token, char delim)
    {
        Tokens tokens;
        std::string::size_type start = 0;
        std::string::size_type end = std::string::npos;

        while ((end = token.find(delim, start)) != std::string::npos)
        {
            if (end != start)
            {
                tokens.emplace_back(token.substr(start, end - start));
            }
            start = end + 1;
        }
        if (start != token.size())
        {
            tokens.emplace_back(token.substr(start));
        }
        return tokens;
    }

    Tokens split_if_not_quote(QuoteTokens const &quote_tokens)
    {
        Tokens tokens;
        for (auto const &quote_token : quote_tokens)
        {
            if (quote_token.first)
            {
                tokens.emplace_back(quote_token.second);
            }
            else
            {
                auto split_tokens = split(quote_token.second, ' ');
                tokens.insert(tokens.end(), split_tokens.begin(), split_tokens.end());
            }
        }
        return tokens;
    }
}

Tokens tokenize(std::string const &line)
{
    return split_if_not_quote(split_quote(line));
}