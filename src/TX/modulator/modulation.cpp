#include "../../../includes/TX/modulator.hpp"

std::vector<std::complex<double>> modulator::QAM_modulation(const int mod_order,const std::vector<int16_t>& bits){
    int bits_per_symbol = static_cast<int>(std::log2(mod_order));

    std::vector<std::complex<double>> symbols;
    symbols.reserve(bits.size() / bits_per_symbol);

    if(prev_mod_order != mod_order){
        QAM_mapper_table = QAM_mapper_table_generator(mod_order);
        prev_mod_order = mod_order;
    }

    /*get bits block*/
    for (int i= 0; i + bits_per_symbol <= bits.size(); i += bits_per_symbol){
        int map_key = 0;

        /*bits to dec (key of mapper table)*/
        for(int m = 0; m < bits_per_symbol; ++m){
            map_key = (map_key << 1) | (bits[i + m] & 1);
        }

        symbols.push_back(QAM_mapper_table.at(map_key));
    }


    return symbols;
}