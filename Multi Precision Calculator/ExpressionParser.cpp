#include "ExpressionParser.h"
#include <cctype>
#include <stdexcept>

char ExpressionParser::peek() const {
    return pos < expr.size() ? expr[pos] : '\0';
}

char ExpressionParser::consume() {
    return pos < expr.size() ? expr[pos++] : '\0';
}

BigNumber ExpressionParser::parseNumber() {
    std::string num;
    if (peek() == '-') {
        num += consume();
    }
    while (std::isdigit(peek())) {
        num += consume();
    }
    if (peek() == '.') {
        num += consume();
        while (std::isdigit(peek())) {
            num += consume();
        }
    }
    if (num.empty() || num == "-") throw std::runtime_error("Invalid number");
    return BigNumber(num);
}

BigNumber ExpressionParser::parseFactor() {
    if (std::isdigit(peek()) || peek() == '-') {
        return parseNumber();
    }
    else if (peek() == '(') {
        consume();
        BigNumber result = parseExpression();
        if (consume() != ')') throw std::runtime_error("Mismatched parentheses");
        return result;
    }
    throw std::runtime_error("Invalid expression");
}

BigNumber ExpressionParser::parseTerm() {
    BigNumber result = parseFactor();
    while (peek() == '*' || peek() == '/' || peek() == '%') {
        char op = consume();
        BigNumber next = parseFactor();
        if (op == '*') result = result * next;
        else if (op == '/') result = result / next;
        else result = result % next;
    }
    return result;
}

BigNumber ExpressionParser::parseExpression() {
    BigNumber result = parseTerm();
    while (peek() == '+' || peek() == '-') {
        char op = consume();
        BigNumber next = parseTerm();
        if (op == '+') result = result + next;
        else result = result - next;
    }
    return result;
}

BigNumber ExpressionParser::evaluate() {
    pos = 0;
    BigNumber result = parseExpression();
    if (pos < expr.size()) throw std::runtime_error("Invalid expression");
    return result;
}