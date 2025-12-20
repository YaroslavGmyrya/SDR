#include <vector>
#include <complex>
#include "../../../includes/Transmitter.hpp"

//symbols -> samples. Up Fs
std::vector<std::complex<double>> Transmitter::upsampling(const std::vector<std::complex<double>>& symbols){

    std::vector<std::complex<double>> samples(symbols.size() * L, {0,0});
    int j = 0;

    for(int i = 0; i < symbols.size()*L; i+=L){
        samples[i] = symbols[j++];
    }

    return samples;
}