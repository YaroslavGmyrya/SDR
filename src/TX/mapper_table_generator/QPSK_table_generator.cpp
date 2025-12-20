#include <map>
#include <vector>
#include <complex>

#include "../../../includes/Transmitter.hpp"

std::map<int16_t, std::complex<double>> Transmitter::QPSK_mapper_table_generator() {
    std::map<int16_t, std::complex<double>> QPSK_table;

    for (int bits = 0; bits < 4; ++bits) {
        int b0 = (bits >> 1) & 1;
        int b1 = bits & 1;       

        double I = 1/sqrt(2) * (1 - 2*b0);
        double Q = 1/sqrt(2) * (1 - 2*b1);

        QPSK_table[bits] = {I,Q};
    }

    return QPSK_table;
}