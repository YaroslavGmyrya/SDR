#include <iostream>
#include <vector>
#include <complex>
#include <string>

#include "../includes/Transmitter.hpp"
#include "../includes/Receiver.hpp"

int main(int argc, char* argv[]){
    /*get flags*/
    if (argc > 2){
        printf("Error: program wait 1 command line argument, but get %d", argc-1);
        return 1;
    }

    bool logs_enable = std::string(argv[1]) == std::string("-l");

    /*General parameters*/

    const int SPS = 10; //samples per symbol
    const std::vector<double> impulse_response = {1,1,1,1,1,1,1,1,1,1}; //pulse-shaping/match filter impulse response

    /*TX parameters*/
    
    const std::string TX_message("Mr. and Mrs. Smith have one son and one daughter. The son's name is John. The daughter's name is Sarah. The Smiths live in a house. They have a living room. They watch TV in the living room. The father cooks food in the kitchen. They eat in the dining room. The house has two bedrooms. They sleep in the bedrooms. They keep their clothes in the closet. There is one bathroom. They brush their teeth in the bathroom. The house has a garden. John and Sarah play in the garden. They have a dog. John and Sarah like to play with the dog.");
    const std::string pcm_file("../pcm/tx_samples.pcm"); //TX write samples in this file, RX read samples from file
    const std::string modulation_type("QPSK"); //modulation/demodulation type

    /*TX parameters*/
    
    Transmitter TX(TX_message, pcm_file, modulation_type, SPS, impulse_response);

    TX.processing(); //generate samples

    Receiver RX(pcm_file, modulation_type, SPS, impulse_response);

    RX.processing(); //processing samples

    if(logs_enable){
        /*print log info*/
        TX.get_info();
        RX.get_info();
    }
    return 0;
}