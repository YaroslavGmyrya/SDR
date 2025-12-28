#include <vector>
#include <complex>

#include "../../../includes/RX/demodulator.hpp"

std::vector<int16_t> demodulator::QAM_demodulator(const std::vector<std::complex<double>>& symbols, const int mod_order){
    std::vector<int16_t> bits;
    int bits_per_symbol = static_cast<int>(std::log2(mod_order));

    for (int i = 0; i < symbols.size(); ++i) {
        int bit_pattern = QAM_demapper_table[symbols[i]];

        for (int j = bits_per_symbol-1; j >= 0; --j)
            bits.push_back((bit_pattern >> j) & 1);        
    }

    return bits;
}
