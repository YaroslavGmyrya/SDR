#pragma once

#include <complex>

struct ComplexLess {
    bool operator()(const std::complex<double>& a,
                    const std::complex<double>& b) const
    {
        if (a.real() < b.real()) return true;
        if (a.real() > b.real()) return false;
        return a.imag() < b.imag();
    }
};