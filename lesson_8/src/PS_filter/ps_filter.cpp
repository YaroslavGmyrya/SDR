#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
// pulse_shaping filter

int16_t* ps_filter(int* samples, int samples_count, int L, double* g, int* size){
    printf("###################### PS FILTER ##############################\n");
    *size = samples_count;
    int16_t* new_samples = (int16_t*)malloc(samples_count * sizeof(int)); 
    for(int n = 0; n < samples_count; ++n){
        double tmp = 0;
        for(int m = 0; m < L; ++m){
            if (n - m >= 0){
                tmp += samples[n-m]*g[m];
            }
        }

        new_samples[n] = (int16_t)(tmp * 2047) << 4;
        // printf("%d: %d\n", n, new_samples[n]);

        // if(n % 2 == 0){
        //     new_samples[n] = tmp * 2047 << 4;
        // } else{
        //     new_samples[n] = -(tmp * (2048 << 4));
        // }
    }

    return new_samples;
}
