#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define BITS_COUNT 10
#define TAU 10

uint8_t* stob(char* str, int* out_bits_count) {
    int len = strlen(str);
    *out_bits_count = len * 8;
    
    uint8_t* bits = (uint8_t*)malloc(*out_bits_count * sizeof(uint8_t));
    if (bits == NULL) return NULL;
    
    for (int i = 0; i < len; i++) {
        unsigned char c = str[i];
        for (int j = 0; j < 8; j++) {
            bits[i * 8 + j] = (c >> (7 - j)) & 1;
        }
    }
    
    return bits;
}

int16_t* bits_to_signal(uint8_t* bits, int bits_count, int tx_mtu){

    int16_t* tx_buff = (int16_t*)malloc(sizeof(int16_t) * tx_mtu * 2);

    for (int i = 0; i < bits_count; i += 1)
    {   
        for(int j = i*20; j < i*20 + 20 && j < tx_mtu*2; j+=2){
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


int main(){
    FILE* file = fopen("test.txt", "w");

    if(file == nullptr){
        printf("Error in opening file!!!\n");
        return 1;
    }

    int bits_count;
    uint8_t* bits = stob("Hello World", &bits_count);

    for(int i = 0; i < bits_count; i += 2){
        fprintf(file, "%d ", bits[i]);
    }

    int tx_mtu = bits_count * TAU;

    int16_t* signal = bits_to_signal(bits, bits_count, tx_mtu);


    for(int i = 0; i < tx_mtu * 2; i += 2){
        fprintf(file, "(%d,%d) ", signal[i], signal[i+1]);
    }

    fclose(file);

    return 0;
}