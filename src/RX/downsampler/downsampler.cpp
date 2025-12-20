#include <vector>
#include <complex>

#include "../../../includes/Receiver.hpp"

std::vector<std::complex<double>> Receiver::downsampler(const std::vector<std::complex<double>>& samples, const std::vector<int16_t>& symb_offset){

    if(samples.size()/L != symb_offset.size()){
        std::cout << "Error: samples size != offsets size!\n";
        return {};
    }

    int symbols_size = samples.size() / L;
    
    std::vector<std::complex<double>> symbols(symbols_size);
    
    for(int i = 0; i < symb_offset.size(); ++i){
        symbols[i] = samples[symb_offset[i] + i*L];
    }

    return symbols;

}