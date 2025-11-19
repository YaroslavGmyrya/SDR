#include <stdint.h>
#include <stdlib.h>

int16_t* match_filter(int16_t* samples, int samples_count, int L, double* h, int* size){
    *size = samples_count;
    int16_t* new_samples = (int16_t*)malloc(samples_count * sizeof(int16_t)); 
    for(int n = 0; n < samples_count; ++n){
        double tmp = 0;
        for(int m = 0; m < L; ++m){
            if (n - m >= 0){
                tmp += samples[n-m]*h[m];
            }
        }

        new_samples[n] = (int16_t)(tmp);
      
    }

    return new_samples;
}

int16_t* match_filter(double* samples, int samples_count, int L, double* h, int* size){
    *size = samples_count;
    int16_t* new_samples = (int16_t*)malloc(samples_count * sizeof(int16_t)); 
    for(int n = 0; n < samples_count; ++n){
        double tmp = 0;
        for(int m = 0; m < L; ++m){
            if (n - m >= 0){
                tmp += samples[n-m]*h[m];
            }
        }

        new_samples[n] = (int16_t)(tmp);
      
    }

    return new_samples;
}
