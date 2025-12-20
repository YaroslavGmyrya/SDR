#include <map>
#include <vector>
#include <complex>

#include "../../../includes/Transmitter.hpp"

std::map<int16_t, std::complex<double>> Transmitter::QAM16_mapper_table_generator() {
    std::map<int16_t, std::complex<double>> QAM16_table;

    for (int bits = 0; bits < 16; ++bits) {
        int b0 = (bits >> 3) & 1; 
        int b1 = (bits >> 2) & 1; 
        int b2 = (bits >> 1) & 1;
        int b3 = bits & 1;       

        double I = (1/sqrt(10)) * ((1 - 2*b0) * (2 - (1 - 2*b1)));
        double Q = (1/sqrt(10)) * ((1 - 2*b2) * (2 - (1 - 2*b3)));

        QAM16_table[bits] = {I,Q};
    }

    return QAM16_table;
}