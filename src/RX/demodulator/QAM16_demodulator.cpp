#include <vector>
#include <complex>

#include "../../../includes/Receiver.hpp"

std::vector<int16_t> Receiver::QAM16_demodulator(const std::vector<std::complex<double>>& symbols){
    std::vector<int16_t> bits(4 * symbols.size());

    for (int i = 0; i < symbols.size(); ++i) {
        int bit = QPSK_demapper_table[symbols[i]];

        bits.push_back(bit & 1);
        bits.push_back((bit >> 1) & 1);
        bits.push_back((bit >> 2) & 1);
        bits.push_back((bit >> 3) & 1);        
    }

    return bits;
}