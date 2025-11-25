#include <stdint.h>
#include <stdlib.h>

int* down_sampling(int* samples, int samples_count, int L, int start){
    int symbols_size = samples_count / L - start + 1;

    int* symbols = (int*)malloc(symbols_size * sizeof(int));
    
    for(int i = start; i < symbols_size; ++i){
        symbols[i] = samples[i*L];
    }

    return symbols;

}