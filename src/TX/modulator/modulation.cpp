#include "../../../includes/TX/modulator.hpp"

std::vector<std::complex<double>> modulator::QAM_modulation(const int mod_order,const std::vector<int16_t>& bits){
    std::vector<std::complex<double>> symbols(bits.size() / mod_order);

    if(prev_mod_order != mod_order) QAM_mapper_table = QAM_mapper_table_generator(mod_order);

    int j = 0;

    /*get bits block*/
    for(int i = 0; i < bits.size(); i+=mod_order){
        int map_key = 0;
        /*bits to dec (key of mapper table)*/
        for(int m = 0; m < mod_order; ++m){
            map_key = (map_key << m) | bits[i+m] << m;
        }

        symbols[j++] = QAM_mapper_table[map_key];
    }


    return symbols;
}