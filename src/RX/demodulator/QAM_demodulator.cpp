#include <vector>
#include <complex>

#include "../../../includes/RX/demodulator.hpp"

std::vector<int16_t> demodulator::QAM_demodulator(const std::vector<std::complex<double>>& symbols, const int mod_order){
    std::vector<int16_t> bits;

    for (int i = 0; i < symbols.size(); ++i) {
        int bit = QAM_demapper_table[symbols[i]];

        for(int i = 0; i < mod_order; ++i)
            bits.push_back((bit >> i) >> 1);        
    }

    return bits;
}