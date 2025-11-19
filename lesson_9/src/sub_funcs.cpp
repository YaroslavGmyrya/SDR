#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

#include "../includes/subfuncs.h"

uint8_t* stob(char* str, int* out_bits_count)
{
    // get string len
    int len = strlen(str);

    // 1 char = 8 bit 
    *out_bits_count = len * sizeof(char) * 8;
    
    // massive for bits
    uint8_t* bits = (uint8_t*)malloc(*out_bits_count * sizeof(uint8_t));

    // check pointer
    if (bits == NULL){
        return NULL;
    }
        
    char c;

    // iterate on string
    for (int i = 0; i < len; i++) {
        // get char 
        c = str[i];
        // iterate on bits massive
        for (int j = 0; j < 8; j++) {
            // convert char to bits 
            bits[i * 8 + j] = (c >> (7 - j)) & 1;
        }
    }
    
    return bits;
}

int16_t* bits_to_rect_signal(uint8_t* bits, int bits_count, int tx_mtu)
{

    // allocate memory
    int16_t* tx_buff = (int16_t*)malloc(sizeof(int16_t) * tx_mtu * 2);

    // iterate on bitts
    for (int i = 0; i < bits_count; ++i)
    {   
        // fill tx_buff with samples
        for(int j = i*TAU_ON_ELEMENT; j < i*TAU_ON_ELEMENT + 20 && j < tx_mtu*2; j+=2){
            if(bits[i]){
                tx_buff[j] = 2047 << 4;    // I
                tx_buff[j+1] = -2047 << 4; // Q
            } else{
                tx_buff[j] = 0;      //I
                tx_buff[j+1] = 0;    //Q
            }
        }
    }

    return tx_buff;
}

int16_t* bits_to_triangle_signal(uint8_t* bits, int bits_count, int tx_mtu)
{

    // allocate memory
    int16_t* tx_buff = (int16_t*)malloc(sizeof(int16_t) * tx_mtu * 2);

    // iterate on bitts
    for (int i = 0; i < bits_count; ++i)
    {   
        // fill tx_buff with samples
        for(int j = i*TAU_ON_ELEMENT; j < i*TAU_ON_ELEMENT + 20 && j < tx_mtu*2; j+=2){
            if(bits[i] && j == i*TAU_ON_ELEMENT + 8){
                tx_buff[j] = 2047 << 4;    // I
                tx_buff[j+1] = -2047 << 4; // Q
            } else{
                tx_buff[j] = 0;      //I
                tx_buff[j+1] = 0;    //Q
            }
        }
    }

    return tx_buff;
}

int16_t* parabola_signal(int tx_mtu)
{

    // allocate memory
    int16_t* tx_buff = (int16_t*)malloc(sizeof(int16_t) * tx_mtu * 2);

    float coef = -tx_mtu / 10;

    for (int i = 0; i < 2 * tx_mtu; i+=2)
    {
        tx_buff[i] = int16_t(coef * coef);   // I
        tx_buff[i+1] = int16_t(coef * coef); // Q

        coef += 0.1;
    }

    return tx_buff;
}

int16_t *read_pcm(const char *filename, size_t *sample_count)
{
    FILE *file = fopen(filename, "rb");

    if(file == nullptr)
        return nullptr;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    printf("file_size = %ld\n\n", file_size);
    int16_t *samples = (int16_t *)malloc(file_size);

    *sample_count = file_size / sizeof(int16_t);

    size_t sf = fread(samples, sizeof(int16_t), *sample_count, file);

    if (sf == 0){
        printf("file %s empty!\n", filename);
    }

    fclose(file);

    return samples;
}


int* extract_I(int* symbols, int symbols_size, int* I_size){
    *I_size = symbols_size / 2;
    int* I = (int*)malloc(*I_size * sizeof(int));
    int j = 0;

    for(int i = 0; i < symbols_size; i += 2){
        I[j++] = symbols[i];
    }

    return I;
}

int* extract_Q(int* symbols, int symbols_size, int* Q_size){
    *Q_size = symbols_size / 2;
    int* I = (int*)malloc(*Q_size * sizeof(int));
    int j = 0;

    for(int i = 1; i < symbols_size; i += 2){
        I[j++] = symbols[i];
    }

    return I;
}

int* IQ_merge(int* I_samples, int* Q_samples, int size, int* merge_size){
    int* merged = (int*)malloc(size*2*sizeof(int));
    int j = 0;
    for(int i = 0; i < size*2; i+=2){
        merged[i] = I_samples[j];
        merged[i+1] = Q_samples[j];

        ++j;
    }

    return merged;
}

double max(int16_t* arr, int arr_size){
    double max = -__DBL_MAX__;
    for(int i = 0; i < arr_size; ++i){
        if(arr[i] > max){
            max = arr[i];
        }
    }

    return max;
}

double* down_scaler(int16_t* samples, int samples_count){
    double* down_scale_samples = (double*)malloc(samples_count * sizeof(double));

    double max_value = max(samples, samples_count);

    for(int i = 0; i < samples_count; i+=2){
        down_scale_samples[i] = samples[i] / max_value;
    }

    return down_scale_samples;
}

int16_t* up_scaler(int16_t* samples, int samples_count){

    int16_t* up_scale_samples = (int16_t*)malloc(samples_count * sizeof(int16_t));

    for(int i = 0; i < samples_count; i+=2){
        samples[i] *= 2047 << 4;
    }

    return up_scale_samples;
}

char* btos(int8_t* bits, int bits_size, int* str_len){

    if(bits_size % 8 != 0){
        printf("ERROR: invalid bits size!\n");
        return nullptr;
    }

    *str_len = bits_size / 8;

    char* str = (char*)malloc((bits_size/8) * sizeof(char) + 1);

    for (int i = 0; i < bits_size/8; ++i) {
        int8_t byte = 0;
    
        for (int j = 0; j < 8; ++j) {
            if (bits[i*8+j] == 1) {
                byte |= 1 << (7 - j); 
            }
        }
    
        str[i] = (char)byte;
    }

    str[bits_size / 8] = '\0';

    return str;
}
