/*
    This code read samples from .pcm file and convert samples to message
*/

#include "fftw3.h"
#include <chrono>
#include <complex>
#include <fstream>
#include <iostream>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

#include "../../includes/ImGUI_interface.h"
#include "../../includes/OFDM.hpp"
#include "../../includes/Receiver.hpp"
#include "../../includes/TX/modulator.hpp"
#include "../../includes/TX/overhead_encoder.hpp"
#include "../../includes/find_peaks.hpp"
#include "../../includes/general/subfuncs.hpp"
#include "./fft.hpp"

std::vector<std::complex<double>>
int_to_double(const std::vector<std::complex<int16_t>> &samples) {

  std::vector<std::complex<double>> result;
  result.resize(samples.size());

  for (int i = 0; i < samples.size(); ++i) {
    result[i] = {static_cast<double>(std::real(samples[i])),
                 static_cast<double>(std::imag(samples[i]))};
  }

  return result;
}

// void CFO_correction(std::vector<std::complex<double>> &symbols,
//                     const std::vector<int> &peaks,
//                     int CP_L, int Nc, int Fs)
// {
//   const double Ts = 1 / static_cast<double>(Fs);

//   for (int i = 0; i < static_cast<int>(peaks.size()); ++i)
//   {
//     int start = peaks[i];

//     if (start < 0 || start + Nc + CP_L > symbols.size())
//       continue;

//     std::complex<double> corr(0, 0);

//     for (int j = 0; j < CP_L; ++j)
//     {
//       corr += symbols[start + j] * std::conj(symbols[start + j + Nc]);
//     }

//     double cfo = -std::arg(corr) / (2.0 * M_PI * Nc * Ts);

//     for (int k = start + CP_L; k < start + CP_L + Nc; ++k)
//     {
//       double phi = -2.0 * M_PI * cfo * Ts * k;
//       symbols[k] *= std::exp(std::complex<double>(0, phi));
//     }
//   }
// }

void normalized(std::vector<std::complex<double>> &data) {
  std::complex<double> max;
  double max_abs = -__DBL_MAX__;

  for (int i = 0; i < data.size(); ++i) {
    if (std::abs(data[i]) > max_abs) {
      max_abs = std::abs(data[i]);
      max = data[i];
    }
  }

  for (int i = 0; i < data.size(); ++i) {
    data[i] /= max;
  }
}

void RX_proccesing(rx_cfg &rx_config, sdr_config_t &sdr_config) {

  // std::ofstream file("rx_samples.pcm", std::ios::binary);

  // if (!file)
  // {
  //   return;
  // }

  receiver RX;

  int prev_pilots_count = rx_config.pilots_count;
  int prev_FFT_size = rx_config.FFT_size;
  int prev_guard_size = rx_config.guard_size;

  int ofdm_symb_size = rx_config.FFT_size + rx_config.CP_size;
  int payload_size =
      rx_config.FFT_size - 2 * rx_config.guard_size - rx_config.pilots_count;
  int bits_per_symbol = static_cast<int>(std::log2(rx_config.mod_order));

  rx_config.symb_count = sdr_config.buff_size / ofdm_symb_size;

  int N_bits = rx_config.symb_count * payload_size * bits_per_symbol;

  rx_config.grid = create_ofdm_grid(rx_config.FFT_size, rx_config.pilots_count,
                                    rx_config.guard_size);

  while (1) {

    if (!rx_config.OFDM) {

      std::vector<double> IR(rx_config.sps, 1); // matched filter IR
      int barker_code_size = 13;
      /*RX object*/
      modulator modulator_;
      overhead_encoder overhead_encoder_;

      std::vector<std::complex<double>> samples =
          int_to_double(rx_config.rx_samples);

      /*CFO*/

      RX.synchronizer_.coarse_freq_offset(samples, rx_config,
                                          sdr_config.rx_sample_rate);

      /*FFT*/

      rx_config.spectrum = fft(rx_config.rx_samples, sdr_config.rx_sample_rate);

      // /*generate barker code*/
      // std::vector<int16_t> barker_code =
      //     overhead_encoder_.generate_barker_code(barker_code_size);

      // /*barker code -> symbols*/
      // std::vector<std::complex<double>> barker_code_symb =
      //     modulator_.QAM(rx_config.mod_order, barker_code);

      /*RX work logic*/

      /*coarse freq sync*/
      // std::vector<std::complex<double>> cfo_symbols =
      //     RX.synchronizer_.coarse_freq_offset(samples2, barker_code_size);

      /*send samples to mathced filter to increase SNR*/
      rx_config.mf_samples_out = std::move(downscaler(RX.mf_filter_.convolve(
          rx_config.post_cfo_signal, IR, rx_config.sps)));

      /*symbol sync (gardner scheme). Find offsets for each symbol*/
      std::vector<int16_t> offsets = RX.synchronizer_.gardner(
          rx_config.mf_samples_out, rx_config.sps, rx_config.gardner_Kp,
          rx_config.gardner_BnTs);

      /*samples -> symbols*/
      rx_config.raw_symbols = RX.mf_filter_.downsampling(
          rx_config.mf_samples_out, offsets, rx_config.sps);

      rx_config.post_costas = RX.synchronizer_.costas_loop(
          rx_config.raw_symbols, 1, rx_config.costas_BnTs, rx_config.costas_Kp,
          rx_config.mod_order);

      // /*get corr_coeffs (simulate correlation receiving)*/
      // rx_config.corr_func =
      //     RX.synchronizer_.corr_receiving(rx_config.raw_symbols,
      //     barker_code_symb);

      // /*find peak of correlation (start packet)*/
      // int start_sync = RX.synchronizer_.find_sync_index(corr_coeffs);

      // /*slice symbols*/
      // std::vector<std::complex<double>> slice_symbols(symbols.begin() + 0,
      //                                                 symbols.end());

      // /*fine freq sync*/
      // std::vector<std::complex<double>> post_costas =
      //     RX.synchronizer_.costas_loop(slice_symbols);

      // /*cut barker*/
      // std::vector<std::complex<double>> wo_barker(
      //     post_costas.begin() + barker_code_size, post_costas.end());

      /*symbols -> true symbols (quantization)*/
      // std::vector<std::complex<double>> true_symbols =
      //     RX.demodulator_.QAM_quantizater(rx_config.raw_symbols,
      //     rx_config.mod_order);

      // /*true symbols -> bits*/
      // std::vector<int16_t> bits =
      //     RX.demodulator_.QAM_demodulator(true_symbols, rx_config.mod_order);
    } else {

      // file.write(reinterpret_cast<const char *>(rx_config.rx_samples.data()),
      // rx_config.rx_samples.size() * sizeof(std::complex<int16_t>));

      auto start = std::chrono::high_resolution_clock::now();

      if (rx_config.FFT_size != prev_FFT_size ||
          rx_config.pilots_count != prev_pilots_count ||
          rx_config.guard_size != prev_guard_size) {
        // rx_config.grid = create_ofdm_grid(rx_config.FFT_size,
        // rx_config.pilots_count,
        //                                   rx_config.guard_size);
        if (rx_config.guard_size < rx_config.FFT_size / 3) {

          prev_pilots_count = rx_config.pilots_count;
          prev_FFT_size = rx_config.FFT_size;
          prev_guard_size = rx_config.guard_size;

          ofdm_symb_size = rx_config.FFT_size + rx_config.CP_size;
          payload_size = rx_config.FFT_size - 2 * rx_config.guard_size -
                         rx_config.pilots_count;
          bits_per_symbol = static_cast<int>(std::log2(rx_config.mod_order));

          rx_config.symb_count = sdr_config.buff_size / ofdm_symb_size;

          N_bits = rx_config.symb_count * payload_size * bits_per_symbol;

          rx_config.grid = create_ofdm_grid(
              rx_config.FFT_size, rx_config.pilots_count, rx_config.guard_size);
        }
      }

      std::vector<std::complex<double>> samples_d =
          int_to_double(rx_config.rx_samples);

      std::vector<double> cfo;

      rx_config.corr_func = OFDM_corr_receiving(
          samples_d, cfo, rx_config.FFT_size, rx_config.CP_size);

      rx_config.spectrum = fft(samples_d, sdr_config.rx_sample_rate);

      findPeaks::PeakConditions conditions;
      conditions.set_height(0.9); // Minimum height of 2.0
      conditions.set_prominence(0.7);
      conditions.set_distance(rx_config.FFT_size +
                              rx_config.CP_size); // At least 2 samples between

      std::vector<int> peaks =
          findPeaks::find_peaks(rx_config.corr_func, conditions);

      // std::cout << "\n\n";
      // for (int i = 1; i < peaks.size(); ++i)
      // {
      //   std::cout << peaks[i] - peaks[i - 1] << " ";
      // }

      if (peaks.size() == 0) {
        continue;
      }

      CFO_estimation(samples_d, peaks, rx_config.CP_size, rx_config.FFT_size);

      std::vector<std::complex<double>> rx_symbols = extract_OFDM_symbols(
          samples_d, peaks, rx_config.CP_size, rx_config.FFT_size);

      // std::cout << "\n\nRX grid: ";
      // for (int i = 0; i < rx_config.grid.size(); ++i)
      // {
      //   std::cout << rx_config.grid[i];
      // }

      rx_symbols = batch_fft(rx_symbols, rx_config.FFT_size);

      // normalized(rx_symbols);

      // std::cout << "\n\nSymbols: ";
      // for (int i = 0; i < rx_symbols.size(); ++i)
      // {
      //   std::cout << rx_symbols[i] << " ";
      // }

      rx_config.channel_estimation = channel_estimation(
          rx_symbols, rx_config.grid, rx_config.pilot_value, rx_config);

      // std::cout << "\n\n before interp: ";
      // for (int i = 0; i < rx_config.before_inter.size(); ++i)
      // {
      //   std::cout << rx_config.before_inter[i] << " ";
      // }

      // std::vector<double> x(rx_config.before_inter.size());
      // std::vector<double> y_r(rx_config.before_inter.size());
      // std::vector<double> y_i(rx_config.before_inter.size());

      // for (int i = 0; i < x.size(); ++i)
      // {
      //   x[i] = i + 1;
      // }

      // for (int i = 0; i < rx_config.before_inter.size(); ++i)
      // {

      //   if (std::real(rx_config.before_inter[i]) != 0 &&
      //   std::imag(rx_config.before_inter[i]) != 0)
      //   {

      //     y_r[i] = std::real(rx_config.before_inter[i]);
      //     y_i[i] = std::imag(rx_config.before_inter[i]);
      //   }
      // }

      // tk::spline s_re, s_im;

      // s_re.set_points(x, y_r);
      // s_im.set_points(x, y_i);

      // // std::vector<int> pilots_pos = get_pilots_pos(rx_config.grid);

      // // linear_interpolation2(y_r, pilots_pos, rx_config.FFT_size);
      // // linear_interpolation2(y_i, pilots_pos, rx_config.FFT_size);

      // rx_config.channel_estimation.resize(y_r.size());

      // for (int i = 0; i < y_r.size(); ++i)
      // {
      //   rx_config.channel_estimation[i] =
      //       std::complex<double>(s_re(i), s_im(i));
      // }

      channel_equalization(rx_symbols, rx_config.channel_estimation);

      // std::cout << "\n\nSymbols1: ";
      // for (int i = 0; i < rx_symbols.size(); ++i)
      // {
      //   if (std::real(rx_symbols[i]) > 2 || std::real(rx_symbols[i]) > 2)

      //     std::cout << rx_symbols[i] << " ";
      // }

      rx_config.raw_symbols = extract_symbols(rx_symbols, rx_config.grid);

      // std::cout << "\n\n"
      //           << rx_config.raw_symbols.size();

      // auto end = std::chrono::high_resolution_clock::now();

      // std::chrono::duration<double> diff = end - start;

      // std::cout << "Время выполнения: " << diff.count() << " секунд" <<
      // std::endl;

      // std::cout << "\n\nSymbols2: ";
      // for (int i = 0; i < rx_symbols.size(); ++i)
      // {
      //   if (std::real(rx_symbols[i]) > 2 || std::real(rx_symbols[i]) > 2)
      //     std::cout << rx_symbols[i] << " ";
      // }

      // std::cout << "\n\nSymbols: ";
      // for (int i = 0; i < rx_config.raw_symbols.size(); ++i)
      // {
      //   std::cout << rx_config.raw_symbols[i] << " ";
      // }
    }
  }
}