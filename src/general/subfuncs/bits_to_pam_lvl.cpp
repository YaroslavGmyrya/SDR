double bits_to_pam_lvl(int bits, int levels){
    int gray = bits ^ (bits >> 1);
    return 2*gray - levels + 1;
}