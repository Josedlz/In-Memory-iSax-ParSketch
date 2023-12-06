#include "utils.h"


std::string toBinary(int symbol, int level) {
    std::string binary = "";
    for (int i = level - 1; i >= 0; --i) {
        binary += (symbol & (1 << i)) ? '1' : '0';
    }
    return binary;
}