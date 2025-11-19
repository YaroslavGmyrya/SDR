#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
// pulse_shaping filter

int16_t* ps_filter1(double* samples, int samples_count, int L, double* g, int* size){
    *size = samples_count;
    int16_t* new_samples = (int16_t*)malloc(samples_count * sizeof(int16_t)); 
    for(int n = 0; n < samples_count; ++n){
        double tmp = 0;
        for(int m = 0; m < L; ++m){
            if (n - m >= 0){
                tmp += samples[n-m]*g[m];
            }
        }

       new_samples[n] = (int16_t)(tmp);
    }

    return new_samples;
}

double* ps_filter(double* samples, int samples_count, int L, double* g, int* size){
    *size = samples_count;
    double* new_samples = (double*)malloc(samples_count * sizeof(double)); 
    for(int n = 0; n < samples_count; ++n){
        double tmp = 0;
        for(int m = 0; m < L; ++m){
            if (n - m >= 0){
                tmp += samples[n-m]*g[m];
            }
        }

        new_samples[n] = tmp;
    }

    return new_samples;
}