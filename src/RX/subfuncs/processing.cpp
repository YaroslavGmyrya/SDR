#include "../../../includes/Receiver.hpp"

void Receiver::processing(){
    samples = read_pcm();

    mf_symbols = mf_filter(samples);

    timing_error_offsets = gardner(mf_symbols);

    symbols = downsampler(mf_symbols, timing_error_offsets);

    downscale_symbols = downscaler(symbols);

    quantizater_symbols = BPSK_quantizater(downscale_symbols);

    bits = BPSK_demodulator(quantizater_symbols);

    message = bin2str(bits);

}