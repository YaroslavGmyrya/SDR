#pragma once

#include "RX/complex_less.hpp"
#include "RX/decoder.hpp"
#include "RX/demodulator.hpp"
#include "RX/synchronizer.hpp"
#include "general/filter.hpp"

class receiver{
    public:
        filter mf_filter_;
        decoder decoder_;
        demodulator demodulator_;
        synchronizer synchronizer_;
};
