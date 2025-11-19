#include <stdint.h>

double* ps_filter(double* samples, int samples_count, int L, double* g, int* size);
int16_t* ps_filter1(double* samples, int samples_count, int L, double* g, int* size);
int16_t* match_filter(int16_t* samples, int samples_count, int L, double* h, int* size);
int16_t* match_filter(double* samples, int samples_count, int L, double* h, int* size);
