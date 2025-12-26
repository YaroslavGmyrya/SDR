#include <vector>
#include <complex>

#include "../../../includes/Receiver.hpp"

std::vector<int16_t> Receiver::QPSK_demodulator(const std::vector<std::complex<double>>& symbols){
    std::vector<int16_t> bits;

    for (int i = 0; i < symbols.size(); ++i) {
        int bit = QPSK_demapper_table[symbols[i]];

        bits.push_back((bit >> 1) & 1);
        bits.push_back(bit & 1);
    }

    return bits;
}