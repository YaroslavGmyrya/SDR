#include <stdlib.h>
#include <stdint.h>

double* BPSK_modulation(uint8_t* bits, int bits_count, int* symbols_count){
    *symbols_count = bits_count * 2;

    double* IQ_samples = (double*)malloc(sizeof(double) * bits_count * 2);
    
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
