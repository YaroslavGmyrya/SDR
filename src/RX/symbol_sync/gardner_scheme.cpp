#include <vector>
#include <complex>
#include <cmath>


#include "../../../includes/Receiver.hpp"

std::vector<int16_t> Receiver::gardner(const std::vector<std::complex<double>>& samples){
    double Kp = 4;
    double BnTs = 0.01;

    double zeta = std::sqrt(2) / 2;
    double theta = (BnTs / 10) / (zeta + 1 / (4 * zeta));
    double den = (1 + 2 * zeta * theta + theta * theta) * Kp;

    double K1 = -4 * zeta * theta / den;
    double K2 = -4 * theta * theta / den;

    double p1 = 0;
    double p2 = 0;
    int offset = L/2;
    double e = 0;

    int n = 0;

    std::vector<int16_t> offset_list;

    for (int i = 0; i < samples.size() / L; ++i) {
        n = offset + L * i;

        e = (samples[n + L].real() - samples[n].real()) * samples[n + L/2].real() +
                   (samples[n + L].imag() - samples[n].imag()) * samples[n + L/2].imag();

        p1 = e * K1;
        p2 += p1 + e * K2;

        p2 -= std::floor(p2);

        offset = static_cast<int>(std::round(p2 * L));
        offset_list.push_back(offset);
    }

    return offset_list;
}