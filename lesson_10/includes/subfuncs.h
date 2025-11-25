#include <stdint.h>
#include <iostream>
#include <cstdio> 

#define ADC_CAPACITY 12
#define BITS_SHIFT 4
#define TAU 10
#define TAU_ON_ELEMENT 20
#define MESSAGE "HHJOHVIDOISFJVDVSDBNSGSFBGDSFBLDDASFJVADSFBJFADVSFBFSAKFSKAFSBNFAVFKSNKFKSBNBAKFSBNFAKSBNFAMKSBNFVALMSBFNFKAVLMKSBFAFSBNKAVKSBF"
#define PATH_TO_AUDIO "../audio_converter/audio_bin.pcm"
#define PATH_TO_PCM "bpsk_samples.pcm"

uint8_t* stob(char* str, int* out_bits_count);
char* btos(int8_t* bits, int bits_size, int* str_len);
int16_t* bits_to_rect_signal(uint8_t* bits, int bits_count, int tx_mtu);
int16_t* parabola_signal(int tx_mtu);
int16_t* bits_to_triangle_signal(uint8_t* bits, int bits_count, int tx_mtu);
int16_t *read_pcm(const char *filename, size_t *sample_count);
int* extract_I(int* symbols, int symbols_size, int* I_size);
int* extract_Q(int* symbols, int symbols_size, int* Q_size);
int* IQ_merge(int* I_samples, int* Q_samples, int size, int* merge_size);
double* down_scaler(int* samples, int samples_count);
int16_t* up_scaler(int16_t* samples, int samples_count);
int* down_sampling(int* samples, int samples_count, int L, int start);

