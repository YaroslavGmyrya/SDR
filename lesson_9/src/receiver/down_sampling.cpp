#include <stdint.h>
#include <stdlib.h>

int16_t* down_sampling(int16_t* samples, int samples_count, int L, int start){
    int symbols_size = samples_count / L - start + 1;

    int16_t* symbols = (int16_t*)malloc(symbols_size * sizeof(int16_t));
    
    for(int i = start; i < symbols_size; ++i){
        symbols[i] = samples[i*L];
    }

    return symbols;

}