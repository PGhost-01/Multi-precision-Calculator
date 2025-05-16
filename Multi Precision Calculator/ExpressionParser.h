#pragma once
#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H

#include "BigNumber.h"
#include <string>

class ExpressionParser {
private:
    std::string expr;
    size_t pos;
    char peek() const;
    char consume();
    BigNumber parseNumber();
    BigNumber parseFactor();
    BigNumber parseTerm();
    BigNumber parseExpression();

public:
    ExpressionParser(const std::string& e) : expr(e), pos(0) {}
    BigNumber evaluate();
};

#endif