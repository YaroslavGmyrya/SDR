#include <complex>
#include <vector>
#include "../../../includes/Transmitter.hpp"

//upscaling symbols for pluto SDR
std::vector<std::complex<int16_t>> Transmitter::upscaling(const std::vector<std::complex<double>>& psf_symbols){

    std::vector<std::complex<int16_t>> samples(psf_symbols.size());

    for(int i = 0; i < psf_symbols.size(); i++){
        int16_t I = static_cast<int16_t>(psf_symbols[i].real() * 2047) << 4;
        int16_t Q = static_cast<int16_t>(psf_symbols[i].imag() * 2047) << 4;

        samples[i] = {I, Q};
    }

    return samples;
}