#include <iostream>
#include <vector>
#include <complex>
#include <string>

#include "../includes/Receiver.hpp"

int main(int argc, char* argv[]){
    if (argc > 2){
        printf("Error: program wait 1 command line argument, but get %d", argc-1);
        return 1;
    }

    bool logs_enable = argc == 2 && std::string(argv[1]) == std::string("-l");

    const std::string filename("../src/TX/pcm/tx_samples.pcm");

    const std::string demodulation_type("BPSK");

    const int L = 10;
    const std::vector<double> impulse_response = {1,1,1,1,1,1,1,1,1,1};

    Receiver RX(filename, demodulation_type, L, impulse_response);

    RX.processing();

    if(logs_enable){
        printf("######################## Parameter after proceing: #############################\n");
        RX.get_info();
    }

    return 0;
}