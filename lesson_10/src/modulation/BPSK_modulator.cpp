#include <stdlib.h>
#include <stdint.h>

int16_t* BPSK_modulation(uint8_t* bits, int bits_count, int* symbols_count){
    *symbols_count = bits_count * 2;

    int16_t* IQ_samples = (int16_t*)malloc(sizeof(int16_t) * bits_count * 2);
    
    int j = 0;
    for(int i = 0; i < bits_count * 2; i+=2){
        if(bits[j] == 1.0){
            IQ_samples[i] = 1.0;
            IQ_samples[i + 1] = 0.0;
        }else{
            IQ_samples[i] = -1.0;
            IQ_samples[i + 1] = 0.0;
        }

        ++j;
    }
    return IQ_samples;
}
