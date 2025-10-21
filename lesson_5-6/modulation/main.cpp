#include <stdlib.h>
#include <stdio.h>
#include "modulation.h"

int main(){
    FILE* bpsk_samples = fopen("bpsk_samples.txt", "w");
    FILE* qpsk_samples = fopen("qpsk_samples.txt", "w");

    if(bpsk_samples == nullptr || qpsk_samples == nullptr){
        return -1;
    }

    int bits_seq[] = {1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0};

    int bits_seq_len = sizeof(bits_seq) / sizeof(int);

    double* bpsk_result = BPSK_modulation(bits_seq, bits_seq_len); 
    double* qpsk_result = QPSK_modulation(bits_seq, bits_seq_len); 

    for(int i = 0; i < bits_seq_len * 2; i+=2){
        fprintf(bpsk_samples,"(%.0f,%.0f), ", bpsk_result[i], bpsk_result[i+1]);
    }

    for(int i = 0; i < bits_seq_len; i+=2){
        fprintf(qpsk_samples,"(%.0f,%.0f), ", qpsk_result[i], qpsk_result[i+1]);
    }

    
    return 0;
}