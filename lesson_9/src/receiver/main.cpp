#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../../includes/filters.h"
#include "../../includes/modulation.h"
#include "../../includes/subfuncs.h"
#include "../../includes/demodulation.h"

int main(){
    
    size_t samples_count = 0;

    int16_t* samples = read_pcm("bpsk_samples.pcm", &samples_count);

    if(samples == nullptr){
        printf("File not found!\n");
        exit(1);
    }

    printf("samples:\n");

    for(int i = 0; i < samples_count; ++i){
        printf("%d ", samples[i]);
    }

    printf("\n");

    int h[] = {1,1,1,1,1,1,1,1,1,1};

    // int bits_count = 0;
    // uint8_t* bits = stob(MESSAGE, &bits_count);

    // printf("bits:\n");

    // for(int i = 0; i < bits_count; ++i){
    //     printf("%d ", bits[i]);
    // }

    // printf("\n");

    // int symbs_count = 0;
    // double* symbs = BPSK_modulation(bits, bits_count, &symbs_count);

    // printf("symbs: %d\n", symbs_count);

    // for(int i = 0; i < symbs_count; ++i){
    //     printf("%f ", symbs[i]);
    // }

    // printf("\n");

    // int up_symbs_count = 0;
    // double* up_symbs = upsampling(symbs, symbs_count, 10, &up_symbs_count);

    // printf("up_sampling: %d\n", up_symbs_count);

    // for(int i = 0; i < up_symbs_count; ++i){
    //     printf("%f ", up_symbs[i]);
    // }

    // printf("\n");

    // int samples_count = 0;
    // double* samples = ps_filter(up_symbs, up_symbs_count, 10, h, &samples_count);

    // printf("Samples:\n");

    // for(int i = 0; i < samples_count; ++i){
    //     printf("%d ", samples[i]);
    // }

    // printf("\n");

    int conv_size = 0;
    int* conv = match_filter(samples, samples_count, 10, h, &conv_size);

    printf("After convolve:\n");

    for(int i = 0; i < conv_size; ++i){
        printf("%d ", conv[i]);
    }

    printf("\n");


    int* ds_samples = down_sampling(conv, samples_count, 10, 0);

    printf("Samples after down sampling:\n");

    for(int i = 0; i < conv_size/10; ++i){
        printf("%d ", ds_samples[i]);
    }

    printf("\n");

    double* down_scale_samples = down_scaler(ds_samples, samples_count/10);

    printf("Samples after downscaling:\n");

    for(int i = 0; i < conv_size/10; ++i){
        printf("%f ", down_scale_samples[i]);
    }

    printf("\n");


    int16_t* q_samples = BPSK_quantizater(down_scale_samples, samples_count/10);

    printf("rounded symbols:\n");

    for(int i = 0; i < samples_count/10; ++i){
        printf("%d ", q_samples[i]);
    }

    printf("\n");

    int8_t* bits1 = BPSK_demodulator(q_samples, samples_count/10);

    printf("bits:\n");

    for(int i = 0; i < samples_count/20; ++i){
        printf("%d ", bits1[i]);
    }

    printf("\n");

    int str_len = 0;
    char* str = btos(bits1, samples_count/20, &str_len);

    printf("string: %s\n", str);

    return 0;
}