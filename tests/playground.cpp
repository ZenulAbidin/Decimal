#include "types/Decimal.h"
#include <string>
#include <iostream>
#include <vector>

int main() {
    std::string input;
    std::vector<std::string> tokens;
    while (true) {
        tokens.clear();
        std::getline(std::cin, input);
        size_t pos = 0;
        std::string token;
        std::string delimiter = " ";
        while ((pos = input.find(delimiter)) != std::string::npos) {
            token = input.substr(0, pos);
            tokens.push_back(token);
            input.erase(0, pos + delimiter.length());
        }
        tokens.push_back(input);
        // ONLY BASIC ARITHMETIC SUPPORT FOR NOW
        if (tokens.size() != 3) {
            std::cout << "Three tokens are required..." << std::endl;
            continue;
        }
        Decimal a, b;
        try {
            a = tokens[0];
            b = tokens[2];
        }
        catch(...) {
            std::cout << "Invalid operands..." << std::endl;
            continue;
        }
        if (tokens[1] == "+") {
            std::cout << a+b << std::endl;
        }
        else if (tokens[1] == "-") {
            std::cout << a-b << std::endl;
        }
        else if (tokens[1] == "*") {
            std::cout << a*b << std::endl;
        }
        else if (tokens[1] == "/") {
            std::cout << a/b << std::endl;
        }
        else {
            std::cout << "Unrecognized operator..." << std::endl;
        }
    }
    return 0;
}
