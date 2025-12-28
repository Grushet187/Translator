#include <iostream>
#include <string>
#include <stdexcept>

#include "translator.h"

int main() {
    Translator tr;

    std::cout << "Enter expression per line. Empty line or EOF to exit.\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {  
            break;
        }
        if (line.empty()) {             
            break;
        }

        try {
            double result = tr.calculate(line);
            std::cout << result << "\n";
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }

    return 0;
}