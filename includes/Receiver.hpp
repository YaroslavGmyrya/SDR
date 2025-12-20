#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <complex>

#include "complex_less.h"

class Receiver{
    private:
        /*parameters*/
        std::string filename;
        std::string demodulation_type;
        std::vector<double> impulse_response;

        std::map<std::complex<double>, int16_t, ComplexLess> BPSK_demapper_table;
        std::map<std::complex<double>, int16_t, ComplexLess> QPSK_demapper_table;
        std::map<std::complex<double>, int16_t, ComplexLess> QAM16_demapper_table;

        int L;

        std::vector<std::complex<int16_t>> samples;
        std::vector<std::complex<double>> mf_symbols;
        std::vector<int16_t> timing_error_offsets;
        std::vector<std::complex<double>> symbols;
        std::vector<std::complex<double>> downscale_symbols;
        std::vector<std::complex<double>> quantizater_symbols;
        std::vector<int16_t> bits;
        std::string message;

        /*functions*/

        std::vector<int16_t> BPSK_demodulator(const std::vector<std::complex<double>>& symbols);
        std::vector<int16_t> QPSK_demodulator(const std::vector<std::complex<double>>& symbols);
        std::vector<int16_t> QAM16_demodulator(const std::vector<std::complex<double>>& symbols);

        std::vector<std::complex<double>> downsampler(const std::vector<std::complex<double>>& samples, const std::vector<int16_t>& symb_offset);
        
        std::vector<std::complex<double>> downscaler(const std::vector<std::complex<double>>& samples);

        std::vector<std::complex<double>> mf_filter(const std::vector<std::complex<int16_t>>& samples);

        std::map<std::complex<double>, int16_t, ComplexLess> BPSK_demapper_table_generator();
        std::map<std::complex<double>, int16_t, ComplexLess> QPSK_demapper_table_generator();
        std::map<std::complex<double>, int16_t, ComplexLess> QAM16_demapper_table_generator();

        std::vector<std::complex<double>> BPSK_quantizater(std::vector<std::complex<double>> symbols);
        std::string bin2str(const std::vector<int16_t>& bits);


        std::vector<int16_t> gardner(const std::vector<std::complex<double>>& samples);

        std::vector<std::complex<int16_t>> read_pcm();

        /*subfuncs*/
        template <typename T>
        void print_vector(const std::vector<T>& vector);

        std::vector<int16_t> dec2bin(const int number, const int bit_format);

    public:
        /*constructor*/
        Receiver(const std::string& _filename, const std::string& _demodulation_type, const int _L, const std::vector<double>& _impulse_response){
            if(_L <= 0) {throw std::invalid_argument("Invalid L (L > 0)");}
            if(_demodulation_type != "BPSK" && _demodulation_type != "QPSK" && _demodulation_type != "QAM16") {throw std::invalid_argument("Invalid demodulation type (BPSK, QPSK, QAM16)");}
            if(_filename.size() == 0) {throw std::invalid_argument("Invalid filename (size must more then 0)");}
            if(_impulse_response.size() == 0) {throw std::invalid_argument("Invalid impulse response (size must more then 0)");}

            filename = _filename;
            demodulation_type = _demodulation_type;
            L = _L;
            impulse_response = _impulse_response;

            if(demodulation_type == "BPSK") BPSK_demapper_table = BPSK_demapper_table_generator();
            if(demodulation_type == "QPSK") QPSK_demapper_table = QPSK_demapper_table_generator();
            if(demodulation_type == "QAM16") QAM16_demapper_table = QAM16_demapper_table_generator();
        }

        /*getters*/
        std::string get_message();
        std::string get_filename();
        std::string get_demodulation_type();
        std::vector<int16_t> get_bits();
        std::vector<std::complex<double>> get_symbols();
        std::vector<std::complex<double>> get_mf_symbols();
        std::vector<std::complex<int16_t>> get_samples();
        std::vector<double> get_impulse_response();
        int get_L();
        void get_info();

        /*setters*/
        void set_L(const int _L);
        void set_filename(const std::string& _message);
        void set_demodulation_type(const std::string& _modulation_type);
       
        //message -> samples
        void processing();
};

template <typename T>
void Receiver::print_vector(const std::vector<T>& vector){
    if(vector.size() == 0){
        std::cout << "Error: empty vector\n";
        return;
    }

    for(const T& el : vector){
        std::cout << el << " ";
    }
}
