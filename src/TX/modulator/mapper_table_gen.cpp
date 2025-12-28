#include "../../../includes/TX/modulator.hpp"
#include "../../../includes/general/subfuncs.hpp"
std::map<int16_t, std::complex<double>> modulator::QAM_mapper_table_generator(const int bits_per_symbol){
    
    int cur_mod_order = std::pow(2, bits_per_symbol);

    /*regenerate table if user reset modulation order*/
    if(prev_mod_order != cur_mod_order) QAM_mapper_table = QAM_mapper_table_generator(bits_per_symbol);

    std::map<int16_t, std::complex<double>> table;

    const int M = 1 << bits_per_symbol;
    const int bits_in_pam_axis = bits_per_symbol / 2;
    const int levels = 1 << bits_in_pam_axis;

    double norm_coeff = 0;
    for (int i = 1; i < levels; i += 2)
        norm_coeff += i * i;

    norm_coeff = std::sqrt(2 * norm_coeff / levels);

    for (int bits = 0; bits < M; ++bits) {

        int I_bits = 0;
        int Q_bits = 0;

        for (int i = 0; i < bits_in_pam_axis; ++i) {
            I_bits = (I_bits << 1) | ((bits >> (bits_per_symbol - 1 - i)) & 1);
            Q_bits = (Q_bits << 1) | ((bits >> (bits_in_pam_axis - 1 - i)) & 1);
        }

        double I = bits_to_pam_lvl(I_bits, levels) / norm_coeff;
        double Q = bits_to_pam_lvl(Q_bits, levels) / norm_coeff;

        table[bits] = { I, Q };
    }

    return table;
}
