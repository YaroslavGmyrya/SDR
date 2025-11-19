#include <stdlib.h>

// int* upsampling(int* symbols, int symbols_count, int L, int* out_size){

//     // L - samples on symbols
//     *out_size = symbols_count * L;
//     int* upsampling_symbols = (int*)malloc((symbols_count * L) * sizeof(int));
//     int cur_pos = 0;

//     //iterate on symbols
//     for(int i = 0; i < symbols_count; ++i){
//         //write origin value
//         upsampling_symbols[cur_pos++] = symbols[i];

//         //insert L zero
//         for(int k = 0; k < L-1; ++k){
//             upsampling_symbols[cur_pos++] = 0;
//         }
//     }

//     return upsampling_symbols;
// }

double* upsampling(double* symbols, int symbols_count, int L, int* out_size){

    // L - samples on symbols
    *out_size = symbols_count * L;
    double* upsampling_symbols = (double*)malloc((symbols_count * L) * sizeof(double));
    int cur_pos = 0;

    //iterate on symbols
    for(int i = 0; i < symbols_count; ++i){
        //write origin value
        upsampling_symbols[cur_pos++] = symbols[i];

        //insert L zero
        for(int k = 0; k < L-1; ++k){
            upsampling_symbols[cur_pos++] = 0;
        }
    }

    return upsampling_symbols;
}