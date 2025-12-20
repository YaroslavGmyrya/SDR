#include <vector>
#include <complex>

#include "../../../includes/Receiver.hpp"

std::string Receiver::get_message() {return message;};
std::string Receiver::get_filename() {return filename;};
std::string Receiver::get_demodulation_type() {return demodulation_type;};
std::vector<double> Receiver::get_impulse_response() {return impulse_response;};
std::vector<int16_t> Receiver::get_bits() {return bits;}; 
std::vector<std::complex<double>> Receiver::get_symbols() {return symbols;};
std::vector<std::complex<double>> Receiver::get_mf_symbols() {return mf_symbols;};
std::vector<std::complex<int16_t>> Receiver::get_samples() {return samples;};
int Receiver::get_L() {return L;};

void Receiver::get_info() {
    std::cout << "message: " << message << "\n\n";
    std::cout << "filename: " << filename << "\n\n";
    std::cout << "demodulation type: " << demodulation_type << "\n\n";

    std::cout << "impulse response: " << "\n";
    print_vector(impulse_response);
    std::cout << "\n\n";

    
    if(demodulation_type == "BPSK"){
        std::cout << "BPSK demapper table:\n";

        for (const auto& [key, value] : BPSK_demapper_table) {
            std::cout << key << " -> ";
            print_vector(dec2bin(value, 1));
            std::cout << "\n";
        }
    }

    else if(demodulation_type == "QPSK"){
        std::cout << "QPSK demapper table:\n";

        for (const auto& [key, value] : QPSK_demapper_table) {
            std::cout << key << " -> ";
            print_vector(dec2bin(value, 2));
            std::cout << "\n";
        }
    }

    else if(demodulation_type == "QAM16"){
        std::cout << "QAM16 demapper table:\n";  

        for (const auto& [key, value] : QAM16_demapper_table) {
            std::cout << key << " -> ";
            print_vector(dec2bin(value, 4));
            std::cout << "\n";
        }
    }

    std::cout << "\n\n";

    std::cout << "samples: ";
    print_vector(samples);
    std::cout << "\n\n";

    std::cout << "mf_symbols: ";
    print_vector(mf_symbols);
    std::cout << "\n\n";

    std::cout << "timing_error_offsets: ";
    print_vector(timing_error_offsets);
    std::cout << "\n\n";

    std::cout << "symbols: ";
    print_vector(symbols);
    std::cout << "\n\n";

    std::cout << "downscale symbols: ";
    print_vector(downscale_symbols);
    std::cout << "\n\n";

    std::cout << "quantizster symbols: ";
    print_vector(quantizater_symbols);
    std::cout << "\n\n";

    std::cout << "bits: ";
    print_vector(bits);
    std::cout << "\n\n";

    std::cout << "message: " << message << "\n\n";
}