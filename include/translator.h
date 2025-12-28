#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include "stack.h"

class Eval {
public:
    double evalRpn(const std::vector<Token>& rpn) const {
        ds::Stack<double> st;

        for (const Token& t : rpn) {
            if (t.type == TokenType::Number) {
                st.push(t.value);
                continue;
            }

            if (t.type != TokenType::Operator) {
                throw std::runtime_error("Eval error: unexpected token in RPN");
            }

            char op = t.opChar();

            if (op == '~') {
                if (st.size() < 1) throw std::runtime_error("Eval error: unary minus needs 1 operand");
                double a = st.top(); st.pop();
                st.push(-a);
                continue;
            }

            if (st.size() < 2) throw std::runtime_error("Eval error: binary operator needs 2 operands");

            double b = st.top(); st.pop();
            double a = st.top(); st.pop();

            switch (op) {
            case '+': st.push(a + b); break;
            case '-': st.push(a - b); break;
            case '*': st.push(a * b); break;
            case '/':
                if (b == 0.0) throw std::runtime_error("Eval error: division by zero");
                st.push(a / b);
                break;
            default:
                throw std::runtime_error("Eval error: unknown operator");
            }
        }

        if (st.size() != 1) throw std::runtime_error("Eval error: invalid expression");
        return st.top();
    }
};

class Translator {
    Lexer lexer;
    Parcer parcer;
    Eval eval;

public:
    double calculate(const std::string& expression) {
        lexer.reset(expression);
        std::vector<Token> rpn = parcer.toRpn(lexer);
        return eval.evalRpn(rpn);
    }
};