#pragma once
#include <string>
#include <stdexcept>
#include <cctype>
#include <cstdlib>
#include "token.h"

class Lexer {
    std::string src;
    size_t pos{ 0 };
    Token prev{ Token::end() }; 
    static bool isSpace(char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    static bool isOpChar(char c) {
        return c == '+' || c == '-' || c == '*' || c == '/';
    }

    void skipSpaces() {
        while (pos < src.size() && isSpace(src[pos])) pos++;
    }

    bool unaryContext() const {
        if (prev.type == TokenType::End) return true;
        if (prev.type == TokenType::Operator) return true;
        if (prev.type == TokenType::LeftParen) return true;
        return false;
    }

public:
    explicit Lexer(std::string s = {}) : src(std::move(s)) {}

    void reset(std::string s) {
        src = std::move(s);
        pos = 0;
        prev = Token::end();
    }

    Token next() {
        skipSpaces();
        if (pos >= src.size()) {
            prev = Token::end();
            return Token::end();
        }

        char c = src[pos];

        if (c == '(') {
            pos++;
            prev = Token::leftParen();
            return prev;
        }
        if (c == ')') {
            pos++;
            prev = Token::rightParen();
            return prev;
        }

        if (isOpChar(c)) {
            pos++;
            if (c == '-' && unaryContext()) {
                prev = Token::op('~'); 
                return prev;
            }
            prev = Token::op(c);
            return prev;
        }
        if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
            const char* begin = src.c_str() + pos;
            char* endPtr = nullptr;

            double v = std::strtod(begin, &endPtr);
            if (endPtr == begin) {
                throw std::runtime_error("Lexer error: invalid number");
            }

            size_t consumed = static_cast<size_t>(endPtr - begin);
            std::string raw = src.substr(pos, consumed);
            pos += consumed;

            prev = Token::number(v, raw);
            return prev;
        }

        throw std::runtime_error(std::string("Lexer error: unexpected character '") + c + "'");
    }
};