#include "BigNumber.h"
#include "ExpressionParser.h"
#include <iostream>
#include <string>

int main() {
    std::string expression;
    std::cout << "Enter expression (e.g., 123.45 + 67.89 * (2 - 1) or 999 / 7): ";
    std::getline(std::cin, expression);

    try {
        ExpressionParser parser(expression);
        BigNumber result = parser.evaluate();
        std::cout << "Result: " << result.toString() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}