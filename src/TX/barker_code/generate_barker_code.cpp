#include <vector>
#include "../../../includes/Transmitter.hpp"

std::vector<int16_t> Transmitter::generate_barker_code(int N){
      switch (N) {
        case 2:  return { 1,  1};
        case 3:  return { 1,  1, -1};
        case 4:  return { 1,  1, -1,  1};
        case 5:  return { 1,  1,  1, -1,  1};
        case 7:  return { 1,  1,  1, -1, -1,  1, -1};
        case 11: return { 1,  1,  1, -1, -1, -1,  1, -1, -1,  1, -1};
        case 13: return { 1,  1,  1,  1,  1, -1, -1,  1,  1, -1,  1, -1,  1};
        default:
            std::cout << "Invalid barker code size. Try again!\n"; 
            return {};
    }
}
