#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <complex>

#include "complex_less.hpp"

class demodulator{
    private:

        /*generate demapper table*/
        /*3GPP TS 38.211 version 18.6.0 Release 18*/

        std::map<std::complex<double>, int16_t, ComplexLess> QAM_demapper_table_generator(const int mod_order);
        std::vector<std::complex<double>> QAM_quantizater(const std::vector<std::complex<double>>& symbols);

    public:
        std::map<std::complex<double>, int16_t, ComplexLess> QAM_demapper_table;
        std::vector<int16_t> QAM_demodulator(const std::vector<std::complex<double>>& symbols, const int mod_order);
};