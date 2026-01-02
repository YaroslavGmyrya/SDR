#include <complex>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../includes/Receiver.hpp"
#include "../includes/TX/modulator.hpp"
#include "../includes/TX/overhead_encoder.hpp"
#include "../includes/general/logger.hpp"
#include "../includes/general/subfuncs.hpp"

int main(int argc, char *argv[]) {
  /*################### RX TEST #################*/

  /*RX parameters*/
  const int mod_order = 4; // point on constellation
  const int SPS = 10;      // samples per symbol
  const int barker_code_size = 13;
  std::vector<double> impulse_response = {
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; // mathced filter impulse response
  std::string RX_log_file = "RX_log.txt";
  std::string RX_samples_file = "/home/yaroslav/Desktop/SDR/pcm/tx_samples.pcm";

  /*RX object*/
  receiver RX;
  modulator modulator_;
  overhead_encoder overhead_encoder_;

  /*object for logging*/
  logger RX_log(RX_log_file);

  /*generate barker code*/
  std::vector<int16_t> barker_code =
      overhead_encoder_.generate_barker_code(barker_code_size);

  /*barker code -> symbols*/
  std::vector<std::complex<double>> barker_code_symb =
      modulator_.QAM_modulation(4, barker_code);

  /*RX work logic*/

  /*read samples from file*/
  std::vector<std::complex<int16_t>> samples = read_pcm(RX_samples_file);

  /*coarse freq sync*/
  std::vector<std::complex<double>> cfo_symbols =
      RX.synchronizer_.coarse_freq_offset(samples, barker_code_size);

  /*send samples to mathced filter to increase SNR*/
  std::vector<std::complex<double>> mf_samples =
      RX.mf_filter_.convolve(cfo_symbols, impulse_response, SPS);

  /*symbol sync (gardner scheme). Find offsets for each symbol*/
  std::vector<int16_t> offsets = RX.synchronizer_.gardner(mf_samples, SPS);

  /*samples -> symbols*/
  std::vector<std::complex<double>> symbols =
      RX.mf_filter_.downsampling(mf_samples, offsets, SPS);

  /*get corr_coeffs (simulate correlation receiving)*/
  std::vector<std::complex<double>> corr_coeffs =
      RX.synchronizer_.corr_receiving(symbols, barker_code_symb);

  /*find peak of correlation (start packet)*/
  int start_sync = RX.synchronizer_.find_sync_index(corr_coeffs);

  /*slice symbols*/
  std::vector<std::complex<double>> slice_symbols(symbols.begin() + 0,
                                                  symbols.end());

  /*fine freq sync*/
  std::vector<std::complex<double>> post_costas =
      RX.synchronizer_.costas_loop(slice_symbols);

  /*cut barker*/
  std::vector<std::complex<double>> wo_barker(
      post_costas.begin() + barker_code_size, post_costas.end());

  /*symbols -> true symbols (quantization)*/
  std::vector<std::complex<double>> true_symbols =
      RX.demodulator_.QAM_quantizater(wo_barker, mod_order);

  /*true symbols -> bits*/
  std::vector<int16_t> bits =
      RX.demodulator_.QAM_demodulator(true_symbols, mod_order);

  /*bits -> message*/
  std::string message = RX.decoder_.bin2str(bits);

  /*logging*/
  RX_log.label("#################### RX_PARAMETERS ###################");

  RX_log.value("SPS", SPS);
  RX_log.value("modulation order: ", mod_order);
  RX_log.value("logs write in: ", RX_log_file);
  RX_log.value("samples read from: ", RX_samples_file);

  RX_log.map("QAM demapper table", RX.demodulator_.QAM_demapper_table);

  RX_log.vector("mf impulse response", impulse_response);

  RX_log.label("#################### RX_LOGIC ###################");

  RX_log.vector("samples", samples);
  RX_log.vector("mf_samples", mf_samples);
  RX_log.vector("offsets", offsets);
  RX_log.vector("symbols", symbols);
  RX_log.vector("barker code", barker_code);
  RX_log.vector("barker symbols", barker_code_symb);
  RX_log.vector("correlation coefficient", corr_coeffs);

  RX_log.value("start sync seq index", start_sync);

  RX_log.vector("slice symbols", slice_symbols);
  RX_log.vector("post cfo symbols", cfo_symbols);

  RX_log.vector("post_costas", post_costas);
  RX_log.vector("true_symbols", true_symbols);
  RX_log.vector("bits", bits);

  RX_log.value("message", message);

  return 0;
}