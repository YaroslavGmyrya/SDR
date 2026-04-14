#include "ImGUI_interface.h"
#include <algorithm>
#include <complex>
#include <fftw3.h>
#include <spdlog/spdlog.h>
#include <vector>

std::vector<std::vector<std::complex<double>>>
batched(const std::vector<std::complex<double>> &data, const int size);

void batch_ifft(std::vector<std::complex<double>> &data,
                std::vector<std::complex<double>> &ifft_out, const int FFT_size);

std::vector<std::complex<double>>
add_CP(const std::vector<std::complex<double>> &samples,
       int FFT_size,
       int CP_size);

std::vector<std::complex<double>>
delete_CP(const std::vector<std::complex<double>> &samples,
          const std::vector<int> &peaks, const int CP_size,
          const int FFT_size);

std::vector<std::complex<double>>
extract_inner_symbols(const std::vector<std::complex<double>> &ofdm_symbols,
                      const std::vector<cell_type> &grid);

void PSS_CFO_CORRECTION(std::vector<std::complex<double>> &samples, const std::vector<int> &peaks, const std::vector<std::complex<double>> &ZC, const double Ts);

std::vector<std::complex<double>> batch_fft(const std::vector<std::complex<double>> &data, int batch_size);
std::vector<double>
OFDM_corr_receive(const std::vector<std::complex<double>> &samples,
                  const int symb_size, const int CP_size);

std::complex<double>
corr(const std::vector<std::complex<int16_t>> &symbols,
     const std::vector<std::complex<int16_t>> &sync_seq_symb);

double norm_corr(const std::vector<std::complex<int16_t>> &symbols,
                 const std::vector<std::complex<int16_t>> &sync_seq_symb);

std::vector<double>
OFDM_corr_receiving(const std::vector<std::complex<double>> &samples,
                    const int FFT_size, const int CP_size);

std::vector<std::complex<double>>
create_ofdm_signal(const std::vector<std::complex<double>> &symbols,
                   const std::vector<cell_type> &grid,
                   const std::complex<double> pilot_value, const int buff_size);
std::vector<cell_type>
create_ofdm_grid(const int FFT_size, const int pilots_count, const int gi_size);

std::vector<std::complex<double>>
extract_symbols(const std::vector<std::complex<double>> &ofdm_symbols,
                const std::vector<cell_type> &grid);

void channel_equalization(std::vector<std::complex<double>> &symbols,
                          const std::vector<std::complex<double>> &estimation);

std::vector<std::complex<double>>
channel_estimation(std::vector<std::complex<double>> &signal,
                   const std::vector<cell_type> &grid,
                   std::complex<double> pilot_value, rx_cfg &rx_config);

void linear_interpolation(std::vector<std::complex<double>> &H,
                          const std::vector<int> &pos, int FFT_size);

void linear_interpolation2(std::vector<double> &H, const std::vector<int> &pos,
                           int FFT_size);
std::vector<int> get_pilots_pos(const std::vector<cell_type> &grid);

void CFO_estimation(std::vector<std::complex<double>> &signal,
                    const std::vector<int> &peaks, const int CP_size,
                    const int FFT_size);

// void CFO_correction(std::vector<std::complex<double>> &samples,
//                     const std::vector<int> &peaks,
//                     const std::vector<double> &cfo, const int Lcp,
//                     const int Nc);

std::vector<std::complex<double>> ZC_gen(const int root, const int FFT_size);

std::vector<std::complex<double>> add_ZC(const std::vector<std::complex<double>> &ofdm_samples, const std::vector<std::complex<double>> &ZC);

std::vector<double> ZC_corr(const std::vector<std::complex<double>> &samples,
                            const std::vector<std::complex<double>> &ZC);
