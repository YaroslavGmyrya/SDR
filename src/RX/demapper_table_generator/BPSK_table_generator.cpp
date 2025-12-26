#include <map>
#include <vector>
#include <complex>

#include "../../../includes/Receiver.hpp"
#include "../../../includes/complex_less.h"

std::map<std::complex<double>, int16_t, ComplexLess> Receiver::BPSK_demapper_table_generator(){
    std::map<std::complex<double>, int16_t, ComplexLess> BPSK_demapper_table;

    for(int bit = 0; bit < 2; ++bit){
        double I = 1/sqrt(2) * (1 - 2*bit);
        double Q = 1/sqrt(2) * (1 - 2*bit);

        BPSK_demapper_table[std::complex<double>(I,Q)] = bit;
    }

    return BPSK_demapper_table;
}