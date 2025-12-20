#include <vector>
#include <complex>
#include <fstream>
#include <iostream>

#include "../../../includes/Transmitter.hpp"

void Transmitter::write2pcm(){
    if(samples.size() == 0){
        std::cout << "Samples is empty!\n";
        return;
    }

    std::ofstream file(filename, std::ios::binary);

    if (!file) {
        return;
    }

    file.write(reinterpret_cast<const char*>(samples.data()), samples.size() * sizeof(std::complex<int16_t>));

    file.close();

    std::cout << "Samples write in " << filename << ".pcm\n\n";
}