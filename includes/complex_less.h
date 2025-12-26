#pragma once

#include <complex>

struct ComplexLess {
    static constexpr double epsilon = 1e-12;  // Очень маленькое значение
    
    bool operator()(const std::complex<double>& a, 
                    const std::complex<double>& b) const {
        // Сравнение с учетом epsilon для вещественной части
        if (std::abs(a.real() - b.real()) > epsilon)
            return a.real() < b.real();
        
        // Если вещественные части "равны", сравниваем мнимые
        if (std::abs(a.imag() - b.imag()) > epsilon)
            return a.imag() < b.imag();
        
        return false;  // Считаем равными
    }
};