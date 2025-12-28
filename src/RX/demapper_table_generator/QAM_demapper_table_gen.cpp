#include <map>
#include <vector>
#include <complex>
#include <iostream>

#include "../../../includes/RX/demodulator.hpp"
#include "../../../includes/RX/complex_less.hpp"
#include "../../../includes/general/subfuncs.hpp"

std::map<std::complex<double>, int16_t, ComplexLess> demodulator::QAM_demapper_table_generator(const int mod_order){
    std::map<std::complex<double>, int16_t, ComplexLess> demapper_table;

    const int M = 1 << mod_order;    
    const int bits_per_axis = mod_order / 2;
    const int levels = 1 << bits_per_axis; 

    double norm = 0.0;
    for (int i = 1; i < levels; i += 2)
        norm += i * i;
    norm = std::sqrt(2 * norm / levels);

    for (int bits = 0; bits < M; ++bits) {

        int I_bits = 0;
        int Q_bits = 0;

        for (int i = 0; i < bits_per_axis; ++i) {
            I_bits = (I_bits << 1) | ((bits >> (mod_order - 1 - i)) & 1);
            Q_bits = (Q_bits << 1) | ((bits >> (bits_per_axis - 1 - i)) & 1);
        }

        double I = bits_to_pam_lvl(I_bits, levels) / norm;
        double Q = bits_to_pam_lvl(Q_bits, levels) / norm;

        demapper_table[{I, Q}] = bits;
    }

    return demapper_table;
}