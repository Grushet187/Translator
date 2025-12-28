#pragma once
#include <vector>
#include <stdexcept>
#include "token.h"
#include "stack.h"
#include "lexer.h"

class Parcer {
    static int priority(const Token& t) {
        if (t.type != TokenType::Operator) return -1;
        switch (t.opChar()) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        case '~': return 3;
        default: return -1;
        }
    }

    static bool rightAssociative(const Token& t) {
        return (t.type == TokenType::Operator && t.opChar() == '~');
    }

public:
    std::vector<Token> toRpn(Lexer& lx) {
        std::vector<Token> out;
        ds::Stack<Token> ops;

        enum State { NeedOperand, NeedOperator };
        State st = NeedOperand;

        for (;;) {
            Token t = lx.next();

            if (st == NeedOperand) {
                if (t.type == TokenType::Number) {
                    out.push_back(t);
                    st = NeedOperator;
                    continue;
                }
                if (t.type == TokenType::LeftParen) {
                    ops.push(t);
                    st = NeedOperand;
                    continue;
                }
                if (t.type == TokenType::Operator && t.opChar() == '~') {
                    ops.push(t);
                    st = NeedOperand;
                    continue;
                }
                throw std::runtime_error("Parser error: operand expected");
            }
            if (t.type == TokenType::Operator) {
                while (!ops.empty() && ops.top().type == TokenType::Operator) {
                    const Token& top = ops.top();

                    int pTop = priority(top);
                    int pCur = priority(t);

                    if (pTop > pCur || (pTop == pCur && !rightAssociative(t))) {
                        out.push_back(top);
                        ops.pop();
                    }
                    else {
                        break;
                    }
                }
                ops.push(t);
                st = NeedOperand;
                continue;
            }

            if (t.type == TokenType::RightParen) {
                bool foundLeft = false;
                while (!ops.empty()) {
                    Token top = ops.top();
                    ops.pop();

                    if (top.type == TokenType::LeftParen) {
                        foundLeft = true;
                        break;
                    }
                    out.push_back(top);
                }
                if (!foundLeft) throw std::runtime_error("Parser error: ')' without matching '('");
                st = NeedOperator;
                continue;
            }

            if (t.type == TokenType::End) {
                if (st == NeedOperand) {
                    throw std::runtime_error("Parser error: incomplete expression");
                }
                while (!ops.empty()) {
                    Token top = ops.top();
                    ops.pop();
                    if (top.type == TokenType::LeftParen) {
                        throw std::runtime_error("Parser error: '(' without matching ')'");
                    }
                    out.push_back(top);
                }
                return out;
            }

            throw std::runtime_error("Parser error: operator expected");
        }
    }
};