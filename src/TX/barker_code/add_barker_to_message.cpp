#include "../../../includes/Transmitter.hpp"

std::vector<int16_t> Transmitter::add_barker_to_message(std::vector<int16_t> bits, std::vector<int16_t> barker_code){
    std::vector<int16_t> new_bits;

    for(int i = 0; i < 2; ++i){
        for(int j = 0; j < barker_code.size(); ++j){
            new_bits.push_back(barker_code[j]);
        }
    }

    for(int i = 0; i < bits.size(); ++i){
        new_bits.push_back(bits[i]);
    }

    return new_bits;
}