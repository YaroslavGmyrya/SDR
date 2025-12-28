#include <vector>
#include <complex>
#include <fstream>
#include <iostream>

#include "../../../includes/general/subfuncs.hpp"

std::vector<std::complex<int16_t>> read_pcm(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cout << "Error in opening file!\n";
        return {};
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    int num_samples = size / sizeof(std::complex<int16_t>);

    std::vector<std::complex<int16_t>> samples(num_samples);

    file.read(reinterpret_cast<char*>(samples.data()), size);

    file.close();

    return samples;
}