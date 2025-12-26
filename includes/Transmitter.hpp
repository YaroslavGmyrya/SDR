#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <complex>


class Transmitter{
    private:
        /*parameters*/
        std::string message;
        std::string filename;
        std::string modulation_type;
        std::vector<double> impulse_response;
        std::map<int16_t, std::complex<double>> BPSK_mapper_table;
        std::map<int16_t, std::complex<double>> QPSK_mapper_table;
        std::map<int16_t, std::complex<double>> QAM16_mapper_table;

        int L;

        std::vector<int16_t> bits;
        std::vector<std::complex<double>> symbols;
        std::vector<std::complex<double>> ups_symbols;
        std::vector<std::complex<double>> psf_symbols;
        std::vector<std::complex<int16_t>> samples;


        /*functions*/
        std::vector<int16_t> str2bin(const std::string& message);

        std::vector<std::complex<double>> BPSK_modulator(const std::vector<int16_t>& bits);
        std::vector<std::complex<double>> QPSK_modulator(const std::vector<int16_t>& bits);
        std::vector<std::complex<double>> QAM16_modulator(const std::vector<int16_t>& bits);

        std::vector<std::complex<double>> upsampling(const std::vector<std::complex<double>>& symbols);

        std::vector<std::complex<double>> ps_filter(const std::vector<std::complex<double>>& samples);

        std::vector<std::complex<int16_t>> upscaling(const std::vector<std::complex<double>>& psf_symbols);

        std::map<int16_t, std::complex<double>> BPSK_mapper_table_generator();
        std::map<int16_t, std::complex<double>> QPSK_mapper_table_generator();
        std::map<int16_t, std::complex<double>> QAM16_mapper_table_generator();

        std::vector<int16_t> generate_barker_code(int N);
        std::vector<int16_t> add_barker_to_message(std::vector<int16_t> bits, std::vector<int16_t> barker_code);

        /*subfuncs*/
        template <typename T>
        void print_vector(const std::vector<T>& vector);

        std::vector<int16_t> dec2bin(int number, int bit_format);


    public:
        /*constructor*/
        Transmitter(const std::string& _message, const std::string& _filename, const std::string& _modulation_type, const int _L, const std::vector<double>& _impulse_response){
            if(_L <= 0) {throw std::invalid_argument("Invalid L (L > 0)");}
            if(_modulation_type != "BPSK" && _modulation_type != "QPSK" && _modulation_type != "QAM16") {throw std::invalid_argument("Invalid modulation type (BPSK, QPSK, QAM16)");}
            if(_message.size() == 0) {throw std::invalid_argument("Invalid message (size must more then 0)");}
            if(_filename.size() == 0) {throw std::invalid_argument("Invalid filename (size must more then 0)");}
            if(_impulse_response.size() == 0) {throw std::invalid_argument("Invalid impulse response (size must more then 0)");}

            message = _message;
            filename = _filename;
            modulation_type = _modulation_type;
            L = _L;
            impulse_response = _impulse_response;


            if(modulation_type == "BPSK") BPSK_mapper_table = BPSK_mapper_table_generator();
            if(modulation_type == "QPSK") QPSK_mapper_table = QPSK_mapper_table_generator();
            if(modulation_type == "QAM16") QAM16_mapper_table = QAM16_mapper_table_generator();
        }

        /*getters*/
        std::string get_message();
        std::string get_filename();
        std::string get_modulation_type();
        std::vector<int16_t> get_bits();
        std::vector<std::complex<double>> get_symbols();
        std::vector<std::complex<double>> get_ups_symbols();
        std::vector<std::complex<double>> get_psf_symbols();
        std::vector<std::complex<int16_t>> get_samples();
        std::vector<double> get_impulse_response();
        int get_L();
        void get_info();

        /*setters*/
        void set_L(const int _L);
        void set_message(const std::string& _message);
        void set_modulation_type(const std::string& _modulation_type);
        void set_filename(const std::string& _modulation_type);
        //message -> samples
        void processing();

        void write2pcm();
};

template <typename T>
void Transmitter::print_vector(const std::vector<T>& vector){
    if(vector.size() == 0){
        std::cout << "Error: empty vector\n";
        return;
    }

    for(const T& el : vector){
        std::cout << el << " ";
    }
}
