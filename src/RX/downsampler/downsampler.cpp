#include <vector>
#include <complex>

#include "../../../includes/Receiver.hpp"

std::vector<std::complex<double>> Receiver::downsampler(const std::vector<std::complex<double>>& samples, const std::vector<int16_t>& symb_offset){
    
    std::vector<std::complex<double>> symbols(symb_offset.size());
    
    for(int i = 0; i < symb_offset.size(); ++i){
        symbols[i] = samples[symb_offset[i] + i*L];
    }

    return symbols;

}