#pragma once

#include <iostream>
#include <vector>
#include <complex>

class synchronizer{
    std::vector<int16_t> gardner(const std::vector<std::complex<double>>& samples, int const L);
};