#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cmath>
#include "../../includes/modulation.h"
#include "../../includes/subfuncs.h"

#define T 0.25
#define ALPHA 0.25

int main(){
    FILE* bpsk_samples = fopen("qpsk_samples.pcm", "w");

    //impulse response

    double g[SAMPLES_ON_BIT];

    for(int i = 0; i < SAMPLES_ON_BIT; ++i){
        int t = i - SAMPLES_ON_BIT/2;
        g[i] = cos((M_PI * ALPHA * t) / T) / (1 - pow((2*ALPHA*t/T), 2));
        printf("%d: %lf\n", i, g[i]);
    }


    //translate char to bits
    int bits_count = 0;
    uint8_t* bits = stob(MESSAGE, &bits_count);

    //translate bits to symbols (I, Q)
    int symbols_count = 0;
    int* symbols = QPSK_modulation(bits, bits_count, &symbols_count); 

    //translate IQ to upsampling IQ
    int ups_symbols_count = 0;
    int* ups_symbols = upsampling(symbols, symbols_count, SAMPLES_ON_BIT, &ups_symbols_count);

    //transalte upsampling IQ to samples
    int samples_count = 0;
    int16_t* samples = ps_filter(ups_symbols, ups_symbols_count, SAMPLES_ON_BIT, g, &samples_count);

    // for(int i = 0; i < samples_count; ++i){
    //     printf("%d ", samples[i]);
    // }

    //write samples to file
    fwrite(samples, samples_count * sizeof(int16_t), 1, bpsk_samples);

    fclose(bpsk_samples);
    return 0;
}