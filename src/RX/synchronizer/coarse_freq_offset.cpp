#include <vector>
#include <complex>

#include "../../../includes/RX/synchronizer.hpp"

std::complex<double> to_double(const std::complex<int16_t>& x) {
    return std::complex<double>(static_cast<double>(x.real()), static_cast<double>(x.imag()));
}

std::vector<std::complex<double>> synchronizer::coarse_freq_offset(const std::vector<std::complex<int16_t>>& symbols, const int barker_size){
    double error = 0;
    std::vector<std::complex<double>> recovery_symbols;
    recovery_symbols.reserve(symbols.size());

    // compute coarse freq error
    for (int i = 0; i < barker_size; ++i) {
        error += std::arg(to_double(symbols[i + barker_size]) * std::conj(to_double(symbols[i])));
    }

    error = error / (2 * M_PI * barker_size);

    // correct symbols
    for (size_t i = 0; i < symbols.size(); ++i) {
        recovery_symbols.push_back(to_double(symbols[i]) * std::polar(1.0, -2.0 * M_PI * error));
    }

    return recovery_symbols;
}
