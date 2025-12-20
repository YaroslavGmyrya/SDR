#include <vector>
#include <complex>

#include "../../../includes/Transmitter.hpp"

std::string Transmitter::get_message() {return message;};
std::string Transmitter::get_modulation_type() {return modulation_type;};
std::vector<double> Transmitter::get_impulse_response() {return impulse_response;};
std::vector<int16_t> Transmitter::get_bits() {return bits;}; 
std::vector<std::complex<double>> Transmitter::get_symbols() {return symbols;};
std::vector<std::complex<double>> Transmitter::get_ups_symbols() {return ups_symbols;};
std::vector<std::complex<double>> Transmitter::get_psf_symbols() {return psf_symbols;};
std::vector<std::complex<int16_t>> Transmitter::get_samples() {return samples;};
int Transmitter::get_L() {return L;};

void Transmitter::get_info() {
    std::cout << "message: " << message << "\n\n";

    std::cout << "L: " << L << "\n\n";

    std::cout << "impulse response: ";
    print_vector(impulse_response);
    std::cout << "\n\n";

    if(modulation_type == "BPSK"){
        std::cout << "BPSK demapper table:\n";

        for (const auto& [key, value] : BPSK_mapper_table) {
            print_vector(dec2bin(key, 1));
            std::cout << " -> " << value << "\n";
        }
    }

    else if(modulation_type == "QPSK"){
        std::cout << "QPSK demapper table:\n";

        for (const auto& [key, value] : QPSK_mapper_table) {
            print_vector(dec2bin(key, 2));
            std::cout << " -> " << value << "\n";
        }
    }

    else if(modulation_type == "QAM16"){
        std::cout << "QAM16 demapper table:\n";  

        for (const auto& [key, value] : QAM16_mapper_table) {
            print_vector(dec2bin(key, 4));
            std::cout << " -> " << value << "\n";
        }
    }

    std::cout << "\n\n";

    std::cout << "bits: ";
    print_vector(bits);
    std::cout << "\n\n";

    std::cout << "symbols: ";
    print_vector(symbols);
    std::cout << "\n\n";

    std::cout << "ups_symbols: ";
    print_vector(ups_symbols);
    std::cout << "\n\n";

    std::cout << "psf_symbols: ";
    print_vector(psf_symbols);
    std::cout << "\n\n";

    std::cout << "samples: ";
    print_vector(samples);
    std::cout << "\n\n";
}