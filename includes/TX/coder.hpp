#pragma once

#include <vector>
#include <cstdint>
#include <string>

class coder{
    public:
        std::vector<int16_t> ascii_str2bin(const std::string& message);
};