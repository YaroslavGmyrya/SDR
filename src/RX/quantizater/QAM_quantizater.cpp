#include <vector>
#include <complex>

#include "../../../includes/RX/demodulator.hpp"

std::vector<std::complex<double>> demodulator::QAM_quantizater(const std::vector<std::complex<double>>& symbols, const int mod_order){

    QAM_demapper_table = QAM_demapper_table_generator(mod_order);

    std::vector<std::complex<double>> quantizate_symbols(symbols.size());
    std::vector<std::complex<double>> true_constellation_point;
    
    for(const auto& item : QAM_demapper_table){
        true_constellation_point.push_back(item.first);
    }

    for(int i = 0; i < symbols.size(); ++i){
        double prev_d = __DBL_MAX__;
        double cur_d = 0;
        int index = 0;

        for(int j = 0; j < true_constellation_point.size(); ++j){
            double dx = fabs(true_constellation_point[j].real() - symbols[i].real());
            double dy = fabs(true_constellation_point[j].imag() - symbols[i].imag());

            cur_d = sqrt(pow(dx, 2) + pow(dy, 2));

            if(cur_d < prev_d){
                index = j;
                prev_d = cur_d;
            }
        }

        quantizate_symbols[i] = true_constellation_point[index];
    }

    return quantizate_symbols;
}