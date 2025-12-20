#include <vector>
#include <complex>
#include <cmath>
#include "../../../includes/Transmitter.hpp"

//bits -> QPSK symbols
std::vector<std::complex<double>> Transmitter::QPSK_modulator(const std::vector<int16_t>& bits){
    std::vector<std::complex<double>> symbols(bits.size() / 2);
    
    int j = 0;

    for(int i = 0; i < bits.size(); i+=2){
        int map_key = 0;

        map_key |= bits[i];
        map_key |= bits[i+1] << 1;

        symbols[j++] = QPSK_mapper_table[map_key];
    }


    return symbols;
}