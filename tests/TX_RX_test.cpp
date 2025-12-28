#include <iostream>
#include <vector>
#include <complex>
#include <string>
#include <fstream>

#include "../includes/Transmitter.hpp"
#include "../includes/Receiver.hpp"
#include "../includes/general/subfuncs.hpp"

int main(int argc, char* argv[]){
    /*TX parameters*/
    const int SPS = 10;
    std::string message("YaroslavGmyrya");
    std::vector<double> impulse_response = {1,1,1,1,1,1,1,1,1,1};


    transmitter TX;

    /*TX work logic*/

    std::vector<int16_t> bits = TX.coder_.ascii_str2bin(message);

    std::vector<std::complex<double>> symbols = TX.modulator_.QAM_modulation(4, bits);

    std::vector<std::complex<double>> ups_symbols = TX.filter_.upsampling(symbols, SPS);

    std::vector<std::complex<double>> samples = TX.filter_.convolve(ups_symbols, impulse_response, SPS);

    std::vector<std::complex<int16_t>> scale_samples = upscaling(samples);

    std::ofstream log_file("TX_log.txt");

    log_file << "#################### TX parameters #####################\n\n";
    log_file << "message: " << message << "\n\n";
    log_file << "SPS: " << SPS << "\n\n";
    
    log_file << "impulse response: ";
    for(const double& el : impulse_response)
        log_file << el << " ";
    log_file << "\n\n";

    log_file << "#################### TX work #####################\n\n";

    log_file << "bits: ";
    for(const double& el : bits)
        log_file << el << " ";
    log_file << "\n\n";

    log_file << "symbols: ";
    for(const std::complex<double>& el : symbols)
        log_file << el << " ";
    log_file << "\n\n";

    log_file << "ups_symbols: ";
    for(const std::complex<double>& el : ups_symbols)
        log_file << el << " ";
    log_file << "\n\n";

    log_file << "scale samples: ";
    for(const std::complex<int16_t>& el :scale_samples)
        log_file << el << " ";
    log_file << "\n\n";

    log_file.close();


    return 0;
}