#include <stdlib.h>
#include <stdint.h>

int* QPSK_modulation(uint8_t* bits, int bits_count, int* symbols_count){
    int* IQ_samples = (int*)malloc(sizeof(int) * bits_count);
    *symbols_count = bits_count / 2;
    for(int i = 0; i < bits_count; i+=2){
        if(bits[i]){
            IQ_samples[i] = -1;
            if(bits[i + 1]){
                IQ_samples[i + 1] = -1;
            }else{
                IQ_samples[i + 1] = 1;
            }
        }else{
            IQ_samples[i] = 1;
            if(bits[i + 1]){
                IQ_samples[i + 1] = 1;
            }else{
                IQ_samples[i + 1] = -1;
            }
        }
    }


    return IQ_samples;
}