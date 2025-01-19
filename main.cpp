#include "eval.h"
#include <iostream>

int main() {
    std::string expression;
    std::cout << "enter expression:\n";
    std::cin >> expression;
    try {
        auto res = EvaluateExpression(std::move(expression));
        std::cout << "Evaluated! Result is " << res << '\n';
    } catch (const std::exception& e) {
        std::cerr << "Occured error during evaluating expression\n";
        std::cerr << e.what() << '\n';
    }
    return 0;
}