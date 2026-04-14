/*
    This code convert message to samples and write samples to .pcm file
*/

/*C++ data types and classes*/
#include <complex>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/*spdlog for logging*/
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

/*user functions*/
#include "../../includes/ImGUI_interface.h"
#include "../../includes/OFDM.hpp"
#include "../../includes/Transmitter.hpp"
#include "../../includes/general/subfuncs.hpp"

void TX_proccesing(tx_cfg &config, const sdr_config_t &sdr_cfg) {
  /*tx object*/
  transmitter TX;

  while (1) {

    if (!config.OFDM) {

      int N = (sdr_cfg.buff_size / config.sps) * config.mod_order;

      config.bits = std::move(bits_gen(N));

      int barker_code_size = 4;

      /*create rectangle IR*/
      std::vector<double> IR(config.sps, 1);

      /*TX work logic*/

      /*generate barker code*/
      std::vector<int16_t> barker_code =
          TX.overhead_encoder_.generate_barker_code(barker_code_size);

      /*append sync sequence*/
      std::vector<int16_t> overhead_bits =
          TX.overhead_encoder_.add_sync_seq_to_message(config.bits,
                                                       barker_code);

      /*bits -> QAM symbols*/
      config.symbols =
          std::move(TX.modulator_.QAM(config.mod_order, config.bits));

      /*QAM symbols -> upsampling QAM symbols*/
      std::vector<std::complex<double>> ups_symbols =
          TX.filter_.upsampling(config.symbols, config.sps);

      std::vector<std::complex<double>> samples =
          TX.filter_.convolve(ups_symbols, IR, config.sps);

      /*upsampling QAM symbols -> upscale samples (for pluto SDR)*/
      config.tx_samples = std::move(upscaling(samples));
    } else {

        auto start = std::chrono::steady_clock::now();

        /*=========================================== gen bits ========================================*/
        int ofdm_symb_size = config.FFT_size + config.CP_size;
        int payload_size = config.FFT_size - 2 * config.guard_size - config.pilots_count;
        int bits_per_symbol = static_cast<int>(std::log2(config.mod_order));
        config.symb_count = sdr_cfg.buff_size / ofdm_symb_size - 2;
        int N_bits = config.symb_count * payload_size * bits_per_symbol;

        config.bits = bits_gen(N_bits);


        /*=========================================== gen grid ========================================*/
        config.grid = create_ofdm_grid(config.FFT_size, config.pilots_count, config.guard_size);

        /*=========================================== gen ZC ========================================*/
        std::vector<std::complex<double>> zc = ZC_gen(25, config.FFT_size);

        /*=========================================== gen PSK/QAM symbols ========================================*/
        config.symbols = TX.modulator_.QAM(config.mod_order, config.bits);

        /*=========================================== gen OFDM symbols (Freq Domain) ========================================*/
        std::vector<std::complex<double>> ofdm_symbols = create_ofdm_signal(config.symbols, config.grid, config.pilot_value, sdr_cfg.buff_size);

        /*=========================================== Add ZC ========================================*/
        ofdm_symbols = add_ZC(ofdm_symbols, zc);

        /*=========================================== gen OFDM symbols (Time Domain) ========================================*/
        std::vector<std::complex<double>> ofdm_signal;
        batch_ifft(ofdm_symbols,ofdm_signal, config.FFT_size);

        /*=========================================== Add CP ========================================*/
        ofdm_signal = add_CP(ofdm_signal, config.FFT_size, config.CP_size);

        /*=========================================== Upscaling for SDR ========================================*/
        config.tx_samples = upscaling(ofdm_signal);

        auto end = std::chrono::steady_clock::now();

         auto diff = end - start;
        std::cout << std::chrono::duration<double, std::milli>(diff).count() << " мс" << std::endl;

        }
  }
}