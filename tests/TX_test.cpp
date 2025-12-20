#include <iostream>
#include <vector>
#include <complex>
#include <string>

#include "../includes/Transmitter.hpp"

int main(int argc, char* argv[]){
    if (argc > 2){
        printf("Error: program wait 1 command line argument, but get %d", argc-1);
        return 1;
    }

    bool logs_enable = argc == 2 && std::string(argv[1]) == std::string("-l");

    const std::string message("");
    const std::string filename("../src/TX/pcm/tx_samples.pcm");

    const std::string modulation_type("BPSK");

    const int L = 10;
    const std::vector<double> impulse_response = {1,1,1,1,1,1,1,1,1,1};

    Transmitter TX(message, filename ,modulation_type, L, impulse_response);

    if(logs_enable){
        printf("######################## Init TX with parameters: #############################\n");
        TX.get_info();
    }

    TX.processing();

    if(logs_enable){
        printf("######################## Parameter after proceing: #############################\n");
        TX.get_info();
    }

    TX.write2pcm();

    return 0;
}