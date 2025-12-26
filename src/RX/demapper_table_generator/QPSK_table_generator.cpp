#include <map>
#include <vector>
#include <complex>

#include "../../../includes/Receiver.hpp"
#include "../../../includes/complex_less.h"

std::map<std::complex<double>, int16_t, ComplexLess> Receiver::QPSK_demapper_table_generator() {
    std::map<std::complex<double>, int16_t, ComplexLess> QPSK_demapper_table;

    for (int16_t bits = 0; bits < 4; ++bits) {
        int b0 = (bits >> 1) & 1;
        int b1 = bits & 1;       

        double I = 1/std::sqrt(2) * (1 - 2*b0);
        double Q = 1/std::sqrt(2) * (1 - 2*b1);

        QPSK_demapper_table[{I,Q}] = bits;
    }

    return QPSK_demapper_table;
}