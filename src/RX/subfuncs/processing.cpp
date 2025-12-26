#include "../../../includes/Receiver.hpp"

void Receiver::processing(){
    samples = read_pcm();

    int samples_shift = 0;
    std::vector<std::complex<int16_t>> shift_samples;

    for(int i = samples_shift; i < samples.size(); ++i){
        shift_samples.push_back(samples[i]);
    }

    mf_symbols = mf_filter(shift_samples);

    timing_error_offsets = gardner(mf_symbols);

    symbols = downsampler(mf_symbols, timing_error_offsets);

    downscale_symbols = downscaler(symbols);

    if(demodulation_type == "BPSK"){
        quantizater_symbols = BPSK_quantizater(downscale_symbols);
        bits = BPSK_demodulator(quantizater_symbols);
    }
    else if(demodulation_type == "QPSK"){
        quantizater_symbols = QPSK_quantizater(downscale_symbols);
        bits = QPSK_demodulator(quantizater_symbols);
    }
    else if(demodulation_type == "QAM16"){
        quantizater_symbols = QAM16_quantizater(downscale_symbols);
        bits = QAM16_demodulator(quantizater_symbols);
    }

    message = bin2str(bits);

}