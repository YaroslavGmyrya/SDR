#include <vector>
#include <complex>
#include <cmath>

#include "../../../includes/Receiver.hpp"

//BPSK symbols -> bits
std::vector<int16_t> Receiver::BPSK_demodulator(const std::vector<std::complex<double>>& symbols){
    std::vector<int16_t> bits(symbols.size());
    
    for (int i = 0; i < symbols.size(); ++i) {
        bits[i] = BPSK_demapper_table[symbols[i]];
    }

    return bits;
}