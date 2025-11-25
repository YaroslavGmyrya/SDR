#include <stdint.h>
#include <stdlib.h>
#include <cmath>

int16_t* BPSK_quantizater(double* symbols, int symbols_size){
    int16_t* rounded_symbols = (int16_t*)malloc(symbols_size * sizeof(int16_t));

    int point0[] = {-1, 0};
    int point1[] = {1, 0};

    double distance0;
    double distance1;

    double dx,dy = 0;

    for(int i = 0; i < symbols_size; i+=2){
        dx = point0[0]-symbols[i];
        dy = point0[1]-symbols[i+1];

        distance0 = sqrt(dx*dx + dy*dy);

        dx = point1[0]-symbols[i];
        dy = point1[1]-symbols[i+1];

        distance1 = sqrt(dx*dx + dy*dy);

        if(distance0 > distance1){
            rounded_symbols[i] = 1;
            rounded_symbols[i+1] = 0;
        }else{
            rounded_symbols[i] = -1;
            rounded_symbols[i+1] = 0;
        }
    }

    return rounded_symbols;
    
}