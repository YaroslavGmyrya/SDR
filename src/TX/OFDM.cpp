#include "../../includes/ImGUI_interface.h"
#include <algorithm>
#include <complex>
#include <fftw3.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <vector>
// #include <libInterpolate/Interpolate.hpp>

// std::vector<std::vector<std::complex<double>>> batched(const
// std::vector<std::complex<double>> &data, const int size)
// {
//   if (data.size() % size != 0)
//   {
//     spdlog::error("[OFDM.cpp]: data size % batch size != 0!");
//     return {};
//   }

//   const int batch_count = data.size() / size;
//   std::vector<std::vector<std::complex<double>>> batches;
//   batches.reserve(batch_count);

//   for (int i = 0; i < batch_count; ++i)
//   {
//     auto start = data.cbegin() + (i * size);
//     auto end = data.begin() + ((i + 1) * size);

//     batches.emplace_back(std::vector<std::complex<double>>(start, end));
//   }

//   return batches;
// }

std::vector<std::complex<double>>
batch_ifft(std::vector<std::complex<double>> &data, int batch_size) {
  const int N = data.size();
  if (batch_size <= 0 || N % batch_size != 0)
    return {};

  const int howmany = N / batch_size;
  // std::cout << "HowMany: " << howmany << "\n";
  const int n[] = {batch_size};
  // std::cout << "N: " << N << "\n";

  fftw_complex *in = reinterpret_cast<fftw_complex *>(data.data());
  std::vector<std::complex<double>> out_vec(N);
  fftw_complex *out = reinterpret_cast<fftw_complex *>(out_vec.data());

  fftw_plan plan =
      fftw_plan_many_dft(1, n, howmany, in, nullptr, 1, batch_size, out,
                         nullptr, 1, batch_size, FFTW_BACKWARD, FFTW_MEASURE);

  if (!plan)
    return {};

  fftw_execute(plan);
  fftw_destroy_plan(plan);

  double coeff = std::sqrt(batch_size);

  for (auto &c : out_vec)
    c /= batch_size;

  // std::cout << "SIZE: " << out_vec.size() << "\n";

  return out_vec;
}

std::vector<std::complex<double>>
add_CP(const std::vector<std::complex<double>> &samples,
       const tx_cfg &tx_config, const int ofdm_symb_count) {
  std::vector<std::complex<double>> result;
  result.reserve(samples.size() + ofdm_symb_count * tx_config.CP_size);

  for (int i = 0; i < ofdm_symb_count; ++i) {
    auto symb_begin = samples.begin() + i * tx_config.FFT_size;
    auto symb_end = symb_begin + tx_config.FFT_size;

    auto cp_begin = symb_end - tx_config.CP_size;

    result.insert(result.end(), cp_begin, symb_end);

    result.insert(result.end(), symb_begin, symb_end);
  }

  if (result.size() != 1920) {
    const int padding = 1920 - result.size();
    result.insert(result.end(), padding, {0, 0});
  }

  return result;
}

std::vector<std::complex<double>> BPSK(const std::vector<int16_t> &bits) {
  double norm_coeff = 1 / std::sqrt(2);

  std::vector<std::complex<double>> symbols;
  symbols.reserve(bits.size());

  for (int i = 0; i < bits.size(); ++i) {
    symbols.push_back(
        {norm_coeff * (1 - 2 * bits[i]), norm_coeff * (1 - 2 * bits[i])});
  }

  return symbols;
}

// std::complex<double> corr(const std::vector<std::complex<int16_t>> &symbols,
//                           const std::vector<std::complex<int16_t>>
//                           &sync_seq_symb)
// {
//   if (symbols.size() != sync_seq_symb.size())
//     return {0.0, 0.0};

//   std::complex<double> sum{0.0, 0.0};

//   for (size_t i = 0; i < symbols.size(); ++i)
//   {
//     std::complex<double> x(symbols[i].real(), symbols[i].imag());
//     std::complex<double> y(sync_seq_symb[i].real(), sync_seq_symb[i].imag());
//     sum += x * std::conj(y);
//   }

//   return sum;
// }

// double norm_corr(const std::vector<std::complex<int16_t>> &symbols,
//                  const std::vector<std::complex<int16_t>> &sync_seq_symb)
// {
//   auto unnorm = corr(symbols, sync_seq_symb);

//   double norm_a = 0.0, norm_b = 0.0;
//   for (size_t i = 0; i < symbols.size(); ++i)
//   {
//     std::complex<double> x(symbols[i].real(), symbols[i].imag());
//     std::complex<double> y(sync_seq_symb[i].real(), sync_seq_symb[i].imag());
//     norm_a += std::norm(x);
//     norm_b += std::norm(y);
//   }

//   double norm_coeff = std::sqrt(norm_a * norm_b);
//   if (norm_coeff < 1e-12)
//     return 0.0;

//   return std::abs(unnorm / norm_coeff);
// }

std::vector<double> smooth(const std::vector<double> &x, int w) {
  std::vector<double> y(x.size(), 0.0);
  double acc = 0.0;

  for (int i = 0; i < (int)x.size(); ++i) {
    acc += x[i];
    if (i >= w)
      acc -= x[i - w];
    y[i] = acc / std::min(i + 1, w);
  }
  return y;
}

std::vector<double>
OFDM_corr_receiving(const std::vector<std::complex<double>> &rx,
                    std::vector<double> &cfo, int N, int Lcp) {
  std::vector<double> corr_func;

  const int L = rx.size() - N - Lcp;

  std::complex<double> corr = 0;

  std::vector<std::complex<double>> R;
  std::vector<double> A = {0};
  std::vector<double> B = {0};
  // cfo.reserve(L);

  // double acc = 0;

  for (int k = 0; k < Lcp; ++k) {
    corr += rx[k] * std::conj(rx[k + N]);
    A[0] += std::abs(rx[k]) * std::abs(rx[k]);
    B[0] += std::abs(rx[k + N]) * std::abs(rx[k + N]);
  }

  // cfo.push_back(std::arg(corr) / (2 * M_PI));
  // acc += corr;
  R.push_back(corr);

  double new_a;
  double new_b;

  corr_func.push_back(std::abs(R.back() / std::sqrt(A.back() * B.back())));

  for (int k = 1; k < L; ++k) {
    std::complex<double> newR =
        R.back() - rx[k - 1] * std::conj(rx[k + N - 1]) +
        rx[k + Lcp - 1] * std::conj(rx[k + N + Lcp - 1]);

    // cfo.push_back(std::arg(newR) / (2 * M_PI));

    R.push_back(newR);

    new_a = A.back() - std::norm(rx[k - 1]) + std::norm(rx[k + Lcp - 1]);

    A.push_back(new_a);

    new_b =
        B.back() - std::norm(rx[k + N - 1]) + std::norm(rx[k + N + Lcp - 1]);

    B.push_back(new_b);

    corr_func.push_back(std::abs(R.back() / std::sqrt(A.back() * B.back())));
  }

  return corr_func;
}

void CFO_estimation(std::vector<std::complex<double>> &signal,
                    const std::vector<int> &peaks, const int CP_size,
                    const int FFT_size) {
  std::complex<double> corr(0, 0);
  int start, end;

  for (int i = 0; i < peaks.size(); ++i) {
    start = peaks[i];
    end = start + FFT_size;
    for (int j = 0; j < CP_size; ++j) {
      corr += signal[start + j] * std::conj(signal[end + j]);
    }
  }

  double CFO = -std::arg(corr) / (2 * M_PI);

  for (int i = 0; i < signal.size(); ++i) {
    signal[i] *= std::exp(std::complex<double>(
        0.0, -2.0 * M_PI * CFO * static_cast<double>(i) / FFT_size));
  }
}

std::vector<std::complex<double>>
batch_fft(const std::vector<std::complex<double>> &data, int batch_size) {
  const int N = data.size();

  if (batch_size <= 0 || N % batch_size != 0 || N == 0) {
    // printf("INVALID SIZE");
    return {{0, 0}};
  }

  const int howmany = N / batch_size;
  const int n[] = {batch_size};

  fftw_complex *in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
  fftw_complex *out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);

  if (!in || !out) {
    printf("FFTW malloc failed!\n");
    if (in)
      fftw_free(in);
    if (out)
      fftw_free(out);
    return {};
  }

  for (int i = 0; i < N; ++i) {
    in[i][0] = static_cast<double>(data[i].real());
    in[i][1] = static_cast<double>(data[i].imag());
  }

  fftw_plan plan =
      fftw_plan_many_dft(1, n, howmany, in, nullptr, 1, batch_size, out,
                         nullptr, 1, batch_size, FFTW_FORWARD, FFTW_ESTIMATE);

  if (!plan) {
    printf("FFT dont start!");
    fftw_free(in);
    fftw_free(out);
    return {};
  }

  fftw_execute(plan);

  std::vector<std::complex<double>> result(N);

  for (int i = 0; i < N; ++i) {
    result[i] = {static_cast<double>(out[i][0]),
                 static_cast<double>((out[i][1]))};
  }

  fftw_destroy_plan(plan);
  fftw_free(in);
  fftw_free(out);

  return result;
}

std::vector<std::complex<double>>
extract_OFDM_symbols(const std::vector<std::complex<double>> &ofdm_samples,
                     const std::vector<int> &peaks, const int CP_size,
                     const int Nc) {
  std::vector<std::complex<double>> result;
  result.reserve(peaks.size() * Nc);

  for (int i = 0; i < peaks.size(); ++i) {
    int peak = peaks[i];
    auto start = ofdm_samples.begin() + peak + CP_size;
    auto end = ofdm_samples.begin() + peak + CP_size + Nc;

    result.insert(result.end(), start, end);
  }

  return result;
}

std::vector<std::complex<double>> ofdm_sym_template(std::complex<double> pilot,
                                                    const int FFT_size,
                                                    const int pilots_count,
                                                    const int gi_size) {

  std::vector<std::complex<double>> temp(FFT_size);

  int payload_size = FFT_size - 2 * gi_size;

  int step = payload_size / pilots_count;

  for (int i = gi_size - 1; i < FFT_size - gi_size; i += step) {
    temp[i] = pilot;
  }

  return temp;
}

std::vector<std::complex<double>>
create_ofdm_symbols(const std::vector<std::complex<double>> &symbols,
                    const int Nc, const int gi_size) {
  std::vector<std::complex<double>> ofdm_symbols(symbols.size() + gi_size * 2);

  for (int i = gi_size; i < symbols.size(); ++i) {
    ofdm_symbols[i] = symbols[i - gi_size];
  }
}

std::vector<cell_type> create_ofdm_grid(const int FFT_size,
                                        const int pilots_count,
                                        const int gi_size) {
  // create grid and fill her by zeros
  std::vector<cell_type> grid(FFT_size, data);

  // fill left guard
  for (int i = 0; i < gi_size; ++i) {
    grid[i] = guard;
  }

  // fill right guard
  for (int i = FFT_size - gi_size - 1; i < FFT_size; ++i) {
    grid[i] = guard;
  }

  double pilot_step = double(FFT_size - 2 * gi_size - 1) / (pilots_count - 1);
  int pilot_pos;

  // fill pilots
  for (int i = 0; i < pilots_count; ++i) {
    pilot_pos = gi_size + std::lround(i * pilot_step);
    grid[pilot_pos] = pilot;
  }

  return grid;
}

std::vector<std::complex<double>>
create_ofdm_signal(const std::vector<std::complex<double>> &symbols,
                   const std::vector<cell_type> &grid,
                   const std::complex<double> pilot_value,
                   const int buff_size) {
  std::vector<std::complex<double>> signal;
  signal.reserve(buff_size);

  int k = 0;

  while (k < symbols.size()) {
    for (int j = 0; j < grid.size(); ++j) {

      if (grid[j] == guard) {
        signal.push_back({0, 0});
      }

      else if (grid[j] == pilot) {
        signal.push_back(pilot_value);
      }

      else {
        signal.push_back(symbols[k++]);
      }
    }
  }

  return signal;
}

std::vector<int> get_pilots_pos(const std::vector<cell_type> &grid) {

  std::vector<int> pos;

  for (int i = 0; i < grid.size(); ++i) {
    if (grid[i] == pilot) {
      pos.push_back(i);
    }
  }

  return pos;
}

void linear_interpolation(std::vector<std::complex<double>> &H,
                          const std::vector<int> &pos, int FFT_size) {
  if (pos.size() < 2)
    return;

  int sym = 0;
  bool flag = true;

  while (flag) {
    for (int i = 0; i < pos.size() - 1; ++i) {
      int left_index = pos[i] + sym * FFT_size;
      int right_index = pos[i + 1] + sym * FFT_size;

      if (left_index >= H.size() || right_index >= H.size()) {
        flag = false;
        break;
      }

      std::complex<double> left_point = H[left_index];
      std::complex<double> right_point = H[right_index];

      for (int idx = left_index + 1; idx < right_index; ++idx) {
        double coeff = static_cast<double>(idx - left_index) /
                       static_cast<double>(right_index - left_index);

        H[idx] = left_point + coeff * (right_point - left_point);
      }
    }

    ++sym;
  }
}

void linear_interpolation2(std::vector<double> &H, const std::vector<int> &pos,
                           int FFT_size) {
  if (pos.size() < 2)
    return;

  int sym = 0;
  bool flag = true;

  while (flag) {
    for (int i = 0; i < pos.size() - 1; ++i) {
      int left_index = pos[i] + sym * FFT_size;
      int right_index = pos[i + 1] + sym * FFT_size;

      if (left_index >= H.size() || right_index >= H.size()) {
        flag = false;
        break;
      }

      double left_point = H[left_index];
      double right_point = H[right_index];

      for (int idx = left_index + 1; idx < right_index; ++idx) {
        double coeff = static_cast<double>(idx - left_index) /
                       static_cast<double>(right_index - left_index);

        H[idx] = left_point + coeff * (right_point - left_point);
      }
    }

    ++sym;
  }
}

void unwrap_phase(std::vector<double> &phase, int FFT_size) {
  int total_size = phase.size();
  int symbs_count = total_size / FFT_size;

  for (int s = 0; s < symbs_count; ++s) {
    double offset = 0.0;
    int prev_idx = -1;

    int start = s * FFT_size;
    int end = start + FFT_size;

    for (int i = start; i < end; ++i) {
      if (phase[i] == 0.0)
        continue;

      if (prev_idx == -1) {
        prev_idx = i;
        continue;
      }

      double d = phase[i] - phase[prev_idx];

      if (d > M_PI)
        offset -= 2 * M_PI;
      else if (d < -M_PI)
        offset += 2 * M_PI;

      phase[i] += offset;

      prev_idx = i;
    }
  }
}

std::vector<std::complex<double>>
channel_estimation(std::vector<std::complex<double>> &signal,
                   const std::vector<cell_type> &grid,
                   std::complex<double> pilot_value, rx_cfg &rx_config) {
  std::vector<int> pilots_pos = get_pilots_pos(grid);

  // for (int i = 0; i < pilots_pos.size(); ++i)
  // {
  //   std::cout << pilots_pos[i] << " ";
  // }

  std::cout << "\n\n\n";

  const int FFT_size = grid.size();

  const int symbs_count = signal.size() / FFT_size;

  std::vector<std::complex<double>> estimation(FFT_size * symbs_count);
  std::vector<double> A(FFT_size * symbs_count);
  std::vector<double> phi(FFT_size * symbs_count);
  std::complex<double> sum;
  double cpe;

  for (int i = 0; i < symbs_count; ++i) {
    sum = 0;
    for (int j = 0; j < pilots_pos.size(); ++j) {
      sum += (signal[pilots_pos[j] + i * FFT_size]) * std::conj(pilot_value);
    }

    cpe = std::arg(sum);

    for (int k = FFT_size * i; k < FFT_size * (i + 1); ++k) {
      signal[k] *= std::exp(std::complex<double>(0.0, -cpe));
    }

    for (int j = 0; j < pilots_pos.size(); ++j) {
      A[pilots_pos[j] + i * FFT_size] =
          std::abs(signal[pilots_pos[j] + i * FFT_size] / pilot_value);
      phi[pilots_pos[j] + i * FFT_size] =
          std::arg(signal[pilots_pos[j] + i * FFT_size] / pilot_value);
    }
  }

  unwrap_phase(phi, FFT_size);

  // std::cout << "\n\nBEFORE INTERPOLATION: ";
  // for (int i = 0; i < 64; ++i)
  // {
  //   std::cout << estimation[i] << " ";
  // }

  linear_interpolation2(A, pilots_pos, FFT_size);

  linear_interpolation2(phi, pilots_pos, FFT_size);

  for (int s = 0; s < symbs_count; ++s) {
    int base = s * FFT_size;

    for (int k = 1; k < FFT_size - 1; ++k) {
      A[base + k] = (A[base + k - 1] + A[base + k] + A[base + k + 1]) / 3.0;

      phi[base + k] =
          (phi[base + k - 1] + phi[base + k] + phi[base + k + 1]) / 3.0;
    }
  }

  for (int i = 0; i < A.size(); ++i) {
    estimation[i] = A[i] * std::exp(std::complex<double>(0.0, phi[i]));
  }

  // std::cout << "\n\nAFTER INTERPOLATION: ";

  // for (int i = 0; i < 64; ++i)
  // {
  //   std::cout << estimation[i] << " ";
  // }

  return estimation;
}

void channel_equalization(std::vector<std::complex<double>> &symbols,
                          const std::vector<std::complex<double>> &estimation) {
  for (int i = 0; i < symbols.size(); ++i) {
    if (std::abs(estimation[i]) > 1e-12)
      symbols[i] /= estimation[i];
  }
}

std::vector<std::complex<double>>
extract_symbols(const std::vector<std::complex<double>> &ofdm_symbols,
                const std::vector<cell_type> &grid) {
  std::vector<std::complex<double>> clear_symbols;
  int k = 0;
  bool flag = true;

  while (flag) {
    for (int i = 0; i < grid.size(); ++i) {
      if (i + k * grid.size() >= ofdm_symbols.size()) {
        flag = false;
        break;
      }

      if (grid[i] == data) {
        clear_symbols.push_back(ofdm_symbols[i + k * grid.size()]);
      }
    }

    k++;
  }

  return clear_symbols;
}