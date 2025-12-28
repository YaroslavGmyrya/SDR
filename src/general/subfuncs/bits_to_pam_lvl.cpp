#include "../../../includes/general/subfuncs.hpp"

double bits_to_pam_lvl(int a, int b){
    double gray_code = b ^ (b >> 1);
    return a - 1 - 2 * gray_code;
}