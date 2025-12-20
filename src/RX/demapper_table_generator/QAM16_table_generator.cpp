#include <map>
#include <vector>
#include <complex>
#include <iostream>

#include "../../../includes/Receiver.hpp"
#include "../../../includes/complex_less.h"

std::map<std::complex<double>, int16_t, ComplexLess> Receiver::QAM16_demapper_table_generator() {
    std::map<std::complex<double>, int16_t, ComplexLess> QAM16_demapper_table;

    for (int bits = 0; bits < 16; ++bits) {
        int b0 = (bits >> 3) & 1; 
        int b1 = (bits >> 2) & 1; 
        int b2 = (bits >> 1) & 1;
        int b3 = bits & 1;       

        double I = (1/sqrt(10)) * ((1 - 2*b0) * (2 - (1 - 2*b1)));
        double Q = (1/sqrt(10)) * ((1 - 2*b2) * (2 - (1 - 2*b3)));

        QAM16_demapper_table[{I,Q}] = bits;
    }

    return QAM16_demapper_table;
}