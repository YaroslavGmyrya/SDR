#include "../../../includes/Transmitter.hpp"

void Transmitter::processing(){
    bits = str2bin(message);
    
    if(modulation_type == "BPSK") {
        BPSK_mapper_table = BPSK_mapper_table_generator();
        symbols = BPSK_modulator(bits);
    } 
    
    else if(modulation_type == "QPSK") {
        QPSK_mapper_table = QPSK_mapper_table_generator();
        symbols = QPSK_modulator(bits);
    } 

    else if(modulation_type == "QAM16") {
        QAM16_mapper_table = QAM16_mapper_table_generator();
        symbols = QAM16_modulator(bits);
    } 
    
    ups_symbols = upsampling(symbols);

    psf_symbols = ps_filter(ups_symbols);

    samples = upscaling(psf_symbols);

}