#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "../../includes/modulation.h"
#include "../../includes/subfuncs.h"

int main(){
    FILE* bpsk_samples = fopen("qpsk_samples.pcm", "w");

    //impulse response
    int g[] = {1,1,1,1,1,1,1,1,1,1};

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

    for(int i = 0; i < samples_count; ++i){
        printf("%d ", samples[i]);
    }

    //write samples to file
    fwrite(samples, samples_count * sizeof(int16_t), 1, bpsk_samples);

    fclose(bpsk_samples);
    return 0;
}