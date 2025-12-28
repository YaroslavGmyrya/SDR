#pragma once

#include <iostream>
#include <vector>
#include <complex>

class synchronizer{
    private:
        std::complex<double> corr(const std::vector<std::complex<double>>& symbols, const std::vector<std::complex<double>>& sync_seq_symb);
        double norm_corr(const std::vector<std::complex<double>>& symbols, const std::vector<std::complex<double>>& sync_seq_symb);

    public:
        std::vector<std::complex<double>> coarse_freq_offset(const std::vector<std::complex<int16_t>>& symbols, const int barker_size);
        std::vector<std::complex<double>> costas_loop(const std::vector<std::complex<double>>& samples);
        std::vector<int16_t> gardner(const std::vector<std::complex<double>>& samples, int const L);
        std::vector<std::complex<double>> corr_receiving(const std::vector<std::complex<double>>& symbols, const std::vector<std::complex<double>>& sync_symbols);
        int find_sync_index(const std::vector<std::complex<double>>& corr_coeffs);
};