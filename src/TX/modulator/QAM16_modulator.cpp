#include <vector>
#include <complex>
#include <cmath>
#include "../../../includes/Transmitter.hpp"

//bits -> QAM16 symbols
std::vector<std::complex<double>> Transmitter::QAM16_modulator(const std::vector<int16_t>& bits){
    std::vector<std::complex<double>> symbols(bits.size() / 4);

    int j = 0;

    for(int i = 0; i < bits.size(); i+=4){
        int map_key = 0;

        map_key |= bits[i];
        map_key |= bits[i+1] << 1;
        map_key |= bits[i+2] << 2;
        map_key |= bits[i+3] << 3;

        symbols[j++] = QAM16_mapper_table[map_key];
    }


    return symbols;
}