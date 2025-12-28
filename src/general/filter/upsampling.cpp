#include "../../../includes/general/filter.hpp"

//symbols -> samples. Up Fs
std::vector<std::complex<double>> filter::upsampling(const std::vector<std::complex<double>>& symbols, const int L){
    std::vector<std::complex<double>> samples(symbols.size() * L, {0,0});

    int j = 0;

    for(int i = 0; i < symbols.size()*L; i+=L){
        samples[i] = symbols[j++];
    }

    return samples;
}