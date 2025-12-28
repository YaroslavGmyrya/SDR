#pragma once

#include "general/filter.hpp"
#include "TX/coder.hpp"
#include "TX/modulator.hpp"
#include "TX/overhead_encoder.hpp"

class transmitter{
    public:
        filter filter_;
        coder coder_;
        modulator modulator_;
        overhead_encoder overhead_encoder_;
};
