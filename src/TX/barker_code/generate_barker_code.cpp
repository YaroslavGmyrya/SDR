#include "../../../includes/TX/overhead_encoder.hpp"

std::vector<int16_t> overhead_encoder::generate_barker_code(int len){
      switch (len) {
        case 2:  return { 1,  1};
        case 3:  return { 1,  1, -1};
        case 4:  return { 1,  1, -1,  1};
        case 5:  return { 1,  1,  1, -1,  1};
        case 7:  return { 1,  1,  1, -1, -1,  1, -1};
        case 11: return { 1,  1,  1, -1, -1, -1,  1, -1, -1,  1, -1};
        case 13: return { 1,  1,  1,  1,  1, -1, -1,  1,  1, -1,  1, -1,  1};
        default:
            std::cout << "Invalid barker code size. Try 2,3,4,5,7,11,13!\n"; 
            return {};
    }
}
