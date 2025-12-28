#include <vector>
#include <cmath>

#include "../../../includes/general/subfuncs.hpp"

std::vector<int16_t> dec2bin(const int number, const int bit_format){
    std::vector<int16_t> bits;

    if(number == 0) {
        for(int i = 0; i < bit_format; ++i){
            bits.push_back(0);    
        }

        return bits;
    }

    for(int i = bit_format-1; i >= 0; --i){
        bits.push_back((number >> i) & 1);
    }

    return bits;
}