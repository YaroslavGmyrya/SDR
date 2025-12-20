#include <vector>
#include <complex>
#include "../../../includes/Transmitter.hpp"

//samples -> rect samples 
std::vector<std::complex<double>> Transmitter::ps_filter(const std::vector<std::complex<double>>& samples){
    std::vector<std::complex<double>> psf_samples(samples.size());

    for(int n = 0; n < samples.size(); ++n){
        std::complex<double> tmp = 0;
        for(int m = 0; m < L; ++m){
            if (n - m >= 0){
                tmp += samples[n-m]*impulse_response[m];
            }
        }
        
       psf_samples[n] = tmp;
       
    }

    return psf_samples;
}