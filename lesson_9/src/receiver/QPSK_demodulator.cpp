#include <stdint.h>
#include <stdlib.h>

int8_t* QPSK_demodulator(int16_t* symbols, int symbols_size){
    int bits_size = symbols_size * 2;

    int8_t* bits = (int8_t*)malloc(bits_size * sizeof(int8_t));

    int j = 0;

    for(int i = 0; i < bits_size; i+=2){
        if(symbols[i] == 0 && symbols[i+1] == 0){
            bits[j++] = 1;
        } else{
            bits[j++] = 0;
        }
    }

    return bits;
}