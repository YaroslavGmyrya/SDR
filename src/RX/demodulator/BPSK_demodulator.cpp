#include <vector>
#include <complex>
#include <cmath>

#include "../../../includes/Receiver.hpp"

//BPSK symbols -> bits
std::vector<int16_t> Receiver::BPSK_demodulator(const std::vector<std::complex<double>>& symbols){
    std::vector<int16_t> bits;
    
    for (int i = 0; i < symbols.size(); ++i) {
        bits.push_back(BPSK_demapper_table[symbols[i]]);
    }

    return bits;
}