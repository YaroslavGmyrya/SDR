#include <stdint.h>
#include <iostream>
#include <cstdio> 

#define ADC_CAPACITY 12
#define BITS_SHIFT 4
#define TAU 10
#define TAU_ON_ELEMENT 20
#define MESSAGE "Hello My Beuatiful World AAAAAAAAAAAAAAAAAAAAAGDGWBRWBRWBRRRWVWDBBERHRTedhrwhrerehhfbhrhrejtntghrtgHFBSJBGBDGBDJGBDJKGBKDeghqguwrgwughhgODGGWBRVBWRBVRJVBRBVbefvefbvebvebveBVEBVBVKWVBWKVWVOLBRWVJNggnlwgnwjfewjbgejfbkegWGWGBWGBWOIGBWGNEWGNEWIGBRwnwgwrbgwrbwjerbfeoibgewbgjwebgewbgjWGBWRBGWRBGGEWBGWEJBVWEJRBEWBGEWBGEWGBWERBGWKGBKWVLWLBGOEJGRGBJwobgvwgbvwvnwolvwbgvwrgnowerbgrwgnjwrnvggwbgnWNGWGLORNGWENGEQIGNQEIGBEOGNEIGNQE"
#define PATH_TO_AUDIO "../audio_converter/audio_bin.pcm"
#define PATH_TO_PCM "qpsk_samples.pcm"

uint8_t* stob(char* str, int* out_bits_count);
int16_t* bits_to_rect_signal(uint8_t* bits, int bits_count, int tx_mtu);
int16_t* parabola_signal(int tx_mtu);
int16_t* bits_to_triangle_signal(uint8_t* bits, int bits_count, int tx_mtu);
int16_t *read_pcm(const char *filename, size_t *sample_count);
int* extract_I(int* symbols, int symbols_size, int* I_size);
int* extract_Q(int* symbols, int symbols_size, int* Q_size);
int16_t* ps_filter(int* samples, int samples_count, int L, double* g, int* size);
int* IQ_merge(int* I_samples, int* Q_samples, int size, int* merge_size);
