#include <vector>
#include <complex>

#include "../../../includes/Receiver.hpp"

std::vector<std::complex<double>> Receiver::mf_filter(const std::vector<std::complex<int16_t>>& samples){
    std::vector<std::complex<double>> mf_samples(samples.size());

    for(int n = 0; n < samples.size(); ++n){
        std::complex<double> tmp = 0;
        for(int m = 0; m < L; ++m){
            if (n - m >= 0){
                std::complex<double> x(samples[n-m].real(),samples[n-m].imag());

                tmp += x*impulse_response[m];
            }
        }

        mf_samples[n] = tmp;
      
    }

    return mf_samples;
}