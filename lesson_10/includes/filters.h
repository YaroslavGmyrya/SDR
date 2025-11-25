#include <stdint.h>

double* ps_filter(double* samples, int samples_count, int L, double* g, int* size);
int16_t* ps_filter1(int16_t* samples, int samples_count, int L, int16_t* g, int* size);
// int16_t* match_filter(int16_t* samples, int samples_count, int L, double* h, int* size);
int* match_filter(int16_t* samples, int samples_count, int L, int* h, int* size);
