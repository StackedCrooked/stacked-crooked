#pragma once


#include "Expression.h"
#include "Networking.h"
#include <cstring>
#include <string>


struct Parser
{
    explicit Parser(const char* text);

    Expression parse();

    Expression parse_expression()
    {
        // expr = term { ("and"|"or" term }

        Expression result = parse_term();

        for (;;)
        {
            if (consume_either_token("and", "&&"))
            {
                result = Expression::And(result, parse_term());
            }
            else if (consume_either_token("or", "||"))
            {
                result = Expression::Or(result, parse_term());
            }
            else
            {
                break;
            }
        }

        return result;
    }

    void expect(const char* str)
    {
        if (!consume_text(str))
        {
            error(std::string("Expected: ") + str);
        }
    }

    Expression parse_term()
    {
        if (consume_text("("))
        {
            auto result = parse_expression();
            expect(")");
            return result;
        }
        else
        {
            return parse_attribute();
        }
    }

    Expression parse_attribute()
    {
        if (consume_token("len"))
        {
            if (!consume_text("==") && !consume_text("="))
            {
                return error("equal sign ('=')");
            }

            int len = 0;
            if (!consume_int(len))
            {
                return error("length value");
            }
            return Expression::bpf_length(len);
        }
        else
        {
            return parse_bpf_expression();
        }
    }

    Expression parse_bpf_expression();

    bool consume_eof()
    {
        consume_whitespace();
        return is_eof();
    }

    bool is_eof() const
    {
        return *mText == '\0';
    }




    void consume_whitespace()
    {
        while (is_space(*mText))
        {
            ++mText;
        }
    }

    bool consume_either_token(const char* token1, const char* token2)
    {
        consume_whitespace();
        return consume_token_here(token1) || consume_token_here(token2);
    }

    bool consume_token(const char* token)
    {
        consume_whitespace();
        return consume_token_here(token);
    }

    bool consume_token_here(const char* token)
    {
        auto backup = mText;

        if (!consume_text(token))
        {
            return false;
        }

        if (is_alnum(*mText))
        {
            mText = backup;
            return false;
        }

        return true;
    }

    bool consume_text(const char* token)
    {
        consume_whitespace();
        return consume_text_here(token);
    }

    bool consume_text_here(const char* token)
    {
        return consume_text_impl(token, strlen(token));
    }

    bool consume_text_impl(const char* token, int len)
    {
        if (len > static_cast<int>(mEnd - mText))
        {
            return false;
        }

        if (!strncmp(mText, token, len))
        {
            mText += len;
            return true;
        }

        return false;
    }

    bool consume_int(int& result);

    bool consume_uint8(uint8_t& result);

    bool consume_ip4(IPv4Address& ip);

    bool is_alnum(char c) const
    {
        return is_digit(c) || is_lcase(c) || is_ucase(c);
    }

    bool is_alpha(char c) const
    {
        return is_lcase(c) || is_ucase(c);
    }

    bool is_lcase(char c) const
    {
        return c >= 'a' && c <= 'z';
    }

    bool is_ucase(char c) const
    {
        return c >= 'A' && c <= 'Z';
    }

    bool is_digit(char c) const
    {
        return c >= '0' && c <= '9';
    }

    bool is_space(char c) const
    {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    Expression error(std::string expected = "")
    {
        throw std::runtime_error(std::string(mText - mOriginal, ' ') + "^--- Expected: " + expected);
    }

    const char* const mOriginal;
    const char* mText;
    const char* mEnd;
};

