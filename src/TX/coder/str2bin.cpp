#include "../../../includes/TX/coder.hpp"
#include <string>
#include <cstdint>

//string -> bits
std::vector<int16_t> coder::ascii_str2bin(const std::string& message){

    std::vector<int16_t> bits(message.size() * sizeof(char) * 8);

    // iterate on string
    for (int i = 0; i < message.size(); i++) {
        // iterate on bits massive
        for (int j = 0; j < 8; j++) {
            // convert char to bits 
            bits[i * 8 + j] = (message[i] >> (7 - j)) & 1;
        }
    }
    
    return bits;
}