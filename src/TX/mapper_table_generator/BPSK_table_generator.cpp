#include <map>
#include <vector>
#include <complex>

#include "../../../includes/Transmitter.hpp"

std::map<int16_t, std::complex<double>> Transmitter::BPSK_mapper_table_generator(){
    std::map<int16_t, std::complex<double>> BPSK_mapper_table;

    for(int bit = 0; bit < 2; ++bit){
        double I = 1/sqrt(2) * (1 - 2*bit);
        double Q = 1/sqrt(2) * (1 - 2*bit);

        BPSK_mapper_table[bit] = {I,Q};
    }

    return BPSK_mapper_table;
}