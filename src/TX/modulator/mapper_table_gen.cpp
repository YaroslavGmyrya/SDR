#include "../../../includes/TX/modulator.hpp"
#include "../../../includes/general/subfuncs.hpp"

std::map<int16_t, std::complex<double>> modulator::QAM_mapper_table_generator(const int mod_order) {

    int bits_per_symbol = static_cast<int>(std::log2(mod_order));
    
    std::map<int16_t, std::complex<double>> table;

    const int bits_in_pam_axis = bits_per_symbol / 2;
    const int levels = 1 << bits_in_pam_axis;

    // нормализация
    double norm_coeff = 0;
    for(int i = 0; i < levels; ++i) {
        norm_coeff += (2*i - levels + 1) * (2*i - levels + 1);
    }
    norm_coeff = std::sqrt(norm_coeff*2.0/levels);

    for (int bits = 0; bits < mod_order; ++bits) {

        int I_bits = 0;
        int Q_bits = 0;

        for(int i = 0; i < bits_in_pam_axis; ++i){
            I_bits = (I_bits << 1) | ((bits >> (bits_in_pam_axis + i)) & 1);
            Q_bits = (Q_bits << 1) | ((bits >> i) & 1);
        }

        double I = bits_to_pam_lvl(I_bits, levels) / norm_coeff;
        double Q = bits_to_pam_lvl(Q_bits, levels) / norm_coeff;

        table[bits] = { I, Q };
    }

    return table;
}