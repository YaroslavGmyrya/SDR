#include <map>
#include <vector>
#include <complex>
#include <iostream>

#include "../../../includes/RX/demodulator.hpp"
#include "../../../includes/RX/complex_less.hpp"
#include "../../../includes/general/subfuncs.hpp"

std::map<std::complex<double>, int16_t, ComplexLess> demodulator::QAM_demapper_table_generator(const int mod_order) {
    std::map<std::complex<double>, int16_t, ComplexLess> demapper_table;

    int bits_per_symbol = static_cast<int>(std::log2(mod_order));
    int bits_per_axis = bits_per_symbol / 2;
    int levels = 1 << bits_per_axis;

    double norm = std::sqrt(2.0 / (levels * levels));

    for (int bits = 0; bits < mod_order; ++bits) {
        int I_bits = 0;
        int Q_bits = 0;

        for (int i = 0; i < bits_per_axis; ++i) {
            I_bits = (I_bits << 1) | ((bits >> (bits_per_axis + i)) & 1);
            Q_bits = (Q_bits << 1) | ((bits >> i) & 1);
        }

        double I = bits_to_pam_lvl(I_bits, levels) / std::sqrt(2);
        double Q = bits_to_pam_lvl(Q_bits, levels) / std::sqrt(2);

        demapper_table[{I, Q}] = bits;
    }

    return demapper_table;
}