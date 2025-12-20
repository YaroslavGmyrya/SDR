#include <vector>
#include <complex>
#include <algorithm>
#include <cmath>

#include "../../../includes/Receiver.hpp"

std::vector<std::complex<double>> Receiver::downscaler(const std::vector<std::complex<double>>& samples){
    double rms = 0;
    std::vector<std::complex<double>> downscale_samples(samples.size());

    for (int i = 0; i < samples.size(); ++i)
        rms += pow(samples[i].real(), 2) + pow(samples[i].imag(), 2);

    rms = sqrt(rms / samples.size());

    for (int i = 0; i < downscale_samples.size(); ++i)
        downscale_samples[i] = samples[i] / rms;

    return downscale_samples;
}