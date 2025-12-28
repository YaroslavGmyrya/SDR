#pragma once

#include <iostream>
#include <vector>
#include <complex>
#include <map>

/*bits -> symbols*/
class modulator{
    private:
        /*check modulation order changing*/
        int prev_mod_order;

        /*function for generate mapper table*/
        /*3GPP TS 38.211 version 18.6.0 Release 18*/
        std::map<int16_t, std::complex<double>> QAM_mapper_table_generator(const int bits_per_symbol);

    public:
        std::map<int16_t, std::complex<double>> QAM_mapper_table;

        modulator(){prev_mod_order = -1;}

        std::vector<std::complex<double>> QAM_modulation(const int mod_order, const std::vector<int16_t>& bits);

};