#include <stdlib.h>
#include <stdint.h>

int* BPSK_modulation(uint8_t* bits, int bits_count, int* symbols_count){
    *symbols_count = bits_count * 2;

    int* IQ_samples = (int*)malloc(sizeof(int) * bits_count * 2);
    
    int j = 0;
    for(int i = 0; i < bits_count * 2; i+=2){
        if(bits[j]){
            IQ_samples[i] = 1;
            IQ_samples[i + 1] = 0;
        }else{
            IQ_samples[i] = -1;
            IQ_samples[i + 1] = 0;
        }

        ++j;
    }

    *symbols_count = bits_count;

    return IQ_samples;
}
