#include <vector>
#include <complex>

#include "../../../includes/general/filter.hpp"

std::vector<std::complex<double>> filter::downsampling(const std::vector<std::complex<double>>& samples, const std::vector<int16_t>& symb_offset, const int L){
    
    std::vector<std::complex<double>> symbols;
    
    for(int i = 0; i < symb_offset.size(); ++i){
        symbols.push_back(samples[symb_offset[i] + i*L]);
    }

    return symbols;

}