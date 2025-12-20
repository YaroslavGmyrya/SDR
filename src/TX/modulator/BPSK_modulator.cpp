#include <vector>
#include <complex>
#include <cmath>
#include "../../../includes/Transmitter.hpp"

//bits -> BPSK symbols
std::vector<std::complex<double>> Transmitter::BPSK_modulator(const std::vector<int16_t>& bits){
    std::vector<std::complex<double>> symbols(bits.size());
    
    for(int i = 0; i < bits.size(); ++i){
        symbols[i] = BPSK_mapper_table[bits[i]];
    }

    return symbols;
}