#include <stdlib.h>

double* BPSK_modulation(int* bits, int bits_count){
    double* IQ_samples = (double*)malloc(sizeof(double) * bits_count * 2);

    for(int i,j = 0; i < bits_count * 2; i+=2){
        if(bits[j]){
            IQ_samples[i] = 1;
            IQ_samples[i + 1] = 0;
        }else{
            IQ_samples[i] = -1;
            IQ_samples[i + 1] = 0;
        }

        ++j;
    }

    return IQ_samples;
}
