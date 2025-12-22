#pragma once
#include <string>

enum class TokenType {
    Number,
    Operator,
    LeftParen,
    RightParen,
    End
};
struct Token {
    TokenType type{ TokenType::End };
    std::string text;  
    double value{ 0.0 };

    Token() = default;

    static Token number(double v, std::string raw = {}) {
        Token t;
        t.type = TokenType::Number;
        t.value = v;
        t.text = std::move(raw);
        return t;
    }

    static Token op(char c) {
        Token t;
        t.type = TokenType::Operator;
        t.text.assign(1, c);
        return t;
    }

    static Token leftParen() {
        Token t;
        t.type = TokenType::LeftParen;
        t.text = "(";
        return t;
    }

    static Token rightParen() {
        Token t;
        t.type = TokenType::RightParen;
        t.text = ")";
        return t;
    }

    static Token end() {
        return Token{};
    }

    char opChar() const { return text.empty() ? '\0' : text[0]; }
};