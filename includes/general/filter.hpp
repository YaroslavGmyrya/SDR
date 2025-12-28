#pragma once

#include <vector>
#include <complex>

class filter{
    public:
        std::vector<std::complex<double>> convolve(const std::vector<std::complex<double>>& samples, 
            const std::vector<double>& impulse_response, const int L);

        std::vector<std::complex<double>> upsampling(const std::vector<std::complex<double>>& symbols, const int L);
        std::vector<std::complex<double>> downsampling(const std::vector<std::complex<double>>& samples, 
            const std::vector<int16_t>& symb_offset, const int L);        
};