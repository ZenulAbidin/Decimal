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
        if (std::cin.eof()) {
            break;
        }
        else if (input == "quit") {
            break;
        }
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
        if (tokens.size() != 2) {
            std::cout << "Two tokens are required..." << std::endl;
            continue;
        }
        std::string a;
        try {
	    a = tokens[1];
        }
        catch(...) {
            std::cout << "Invalid operand..." << std::endl;
            continue;
        }
        if (tokens[0] == "hex") {
            std::cout << Decimal(a).ToHex() << std::endl;
        }
        else if (tokens[1] == "dec") {
            std::cout << Decimal::FromHex(a) << std::endl;
        }
        else {
            std::cout << "Unrecognized operator..." << std::endl;
        }
    }
    return 0;
}
