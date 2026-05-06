#include "../../includes/ImGUI_interface.h"
#include <algorithm>
#include <complex>
#include <fftw3.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <vector>
#include "../RX/fft.hpp"
// #include <libInterpolate/Interpolate.hpp>

// std::vector<std::vector<std::complex<double>>> batched(const
// std::vector<std::complex<double>> &data, const int size)
// {
//   if (data.size() % size !=d F 0)
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

using namespace std::complex_literals;

void batch_ifft(std::vector<std::complex<double>> &data,
                std::vector<std::complex<double>> &ifft_out, const int FFT_size)
{

  /*check errors*/
  if (data.size() == 0)
  {
    spdlog::error("[OFDM.cpp:batch_ifft]: The data size is zero!");
    return;
  }

  if (FFT_size <= 0)
  {
    spdlog::error("[OFDM.cpp:batch_ifft]: The FFT_size size is invalid!");
    return;
  }

  if (data.size() % FFT_size != 0)
  {
    spdlog::error("[OFDM.cpp:batch_ifft]: Fractional number of OFDM symbols!");
    return;
  }

  /*count of OFDM symbols*/
  const int howmany = data.size() / FFT_size;

  /*OFDM symbol size*/
  const int n[] = {FFT_size};

  ifft_out.clear();
  ifft_out.resize(FFT_size * howmany);

  /*create FFTW3 complex number (сompatibility with std::complex)*/
  fftw_complex *in = reinterpret_cast<fftw_complex *>(data.data());
  fftw_complex *out = reinterpret_cast<fftw_complex *>(ifft_out.data());

  /*plan is set of settings for FFT
    1 - rank (1D, 2D ...),
    n - size of batch (FFT_size),
    in - fftw_complex array with OFDM in frequency domain,
    FFT_size - space b/w batches,
    out - output array with IFFT result,
    FFTW_BACKWARD - flag for IFFT (FFT have flag FFTW_FORWARD)
  */
  fftw_plan plan =
      fftw_plan_many_dft(1, n, howmany, in, nullptr, 1, FFT_size, out, nullptr,
                         1, FFT_size, FFTW_BACKWARD, FFTW_ESTIMATE);

  /*calculate IFFT*/
  fftw_execute(plan);

  for (auto &x : ifft_out)
  {
    x /= FFT_size;
  }

  /*delete FFTW3 plan*/
  fftw_destroy_plan(plan);
}

std::vector<std::complex<double>>
add_CP(const std::vector<std::complex<double>> &samples,
       int FFT_size,
       int CP_size)
{

  const int symbols_count = samples.size() / FFT_size;

  std::vector<std::complex<double>> result(samples.size() +
                                           symbols_count * CP_size);

  for (int i = 0; i < symbols_count; ++i)
  {
    auto *dst = result.data() + i * (FFT_size + CP_size);
    auto *src = samples.data() + i * FFT_size;

    std::copy_n(src + (FFT_size - CP_size), CP_size, dst);
    std::copy_n(src, FFT_size, dst + CP_size);
  }

  std::cout << result.size() << "\n\n";

  result.resize(1920);

  return result;
}

std::vector<std::complex<double>> BPSK(const std::vector<int16_t> &bits)
{
  double norm_coeff = 1 / std::sqrt(2);

  std::vector<std::complex<double>> symbols;
  symbols.reserve(bits.size());

  for (int i = 0; i < bits.size(); ++i)
  {
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

std::vector<double> smooth(const std::vector<double> &x, int w)
{
  std::vector<double> y(x.size(), 0.0);
  double acc = 0.0;

  for (int i = 0; i < (int)x.size(); ++i)
  {
    acc += x[i];
    if (i >= w)
      acc -= x[i - w];
    y[i] = acc / std::min(i + 1, w);
  }
  return y;
}

std::vector<double>
OFDM_corr_receiving(const std::vector<std::complex<double>> &samples,
                    const int FFT_size, const int CP_size)
{
  std::vector<double> norm_corr;

  /*offset for prevention out of range*/
  const int offset = FFT_size + CP_size - 1;

  /*length of vectors (for normalization)*/
  double A = 0;
  double B = 0;

  /*cur correlation*/
  std::complex<double> corr = 0;

  /*
    Compute full correlation for first symbol.
    if CP element have index k, then original element in end of OFDM symbol have
    index [k+FFT_size]
  */
  for (int k = 0; k < CP_size; ++k)
  {
    corr += samples[k] * std::conj(samples[k + FFT_size]);

    /*|vector| = sqrt(vector[0] ^ 2 + vector[1] ^ 2 ...)*/
    A += std::abs(samples[k]) * std::abs(samples[k]);
    B += std::abs(samples[k + FFT_size]) * std::abs(samples[k + FFT_size]);
  }

  double denom = std::sqrt(A * B);
  norm_corr.push_back(denom > 0 ? std::abs(corr) / denom : 0);

  /*
  update correlation
  We compute correlation b/w two sliding windows. In next iteration we capture
  one element from right side, but loss one element from left side. This means,
  that we can compute correlation recursively:

  corr[n+1] = corr[n-1] + right - left

  For our case:

  corr[k+1] = corr[k] -  samples[k - 1] * conj(samples[k + FFT_size - 1]) +
  samples[k + CP_size - 1] * conj(samples[k + FFT_size + CP_size - 1])

  corr[k] - prev correlation,
  samples[k - 1] * conj(samples[k + FFT_size - 1]) - correlation for loss
  element (left side) samples[k + FFT_size - 1] * conj(samples[k + FFT_size]) -
  correlation for new element (right side)
  */

  for (int k = 1; k < samples.size() - offset; ++k)
  {
    corr = corr - samples[k - 1] * std::conj(samples[k + FFT_size - 1]) +
           samples[k + CP_size - 1] *
               std::conj(samples[k + FFT_size + CP_size - 1]);

    A = A - std::norm(samples[k - 1]) + std::norm(samples[k + CP_size - 1]);

    B = B - std::norm(samples[k + FFT_size - 1]) +
        std::norm(samples[k + FFT_size + CP_size - 1]);

    denom = std::sqrt(A * B);
    norm_corr.push_back(denom > 0 ? std::abs(corr) / denom : 0);
  }

  return norm_corr;
}

void CFO_estimation(std::vector<std::complex<double>> &signal,
                    const std::vector<int> &peaks,
                    int CP_size,
                    int FFT_size, const int Fs)
{
  std::complex<double> corr(0, 0);

  if (peaks.size() == 0)
  {
    return;
  }

  for (int i = 0; i < peaks.size(); ++i)
  {
    int start = peaks[i];
    int end = start + FFT_size;

    if (end + CP_size >= signal.size())
      continue;

    for (int j = 0; j < CP_size; ++j)
    {
      corr += signal[start + j] * std::conj(signal[end + j]);
    }
  }

  double CFO = -std::arg(corr) * Fs / (2 * M_PI * FFT_size);

  std::cout << "CFO: " << CFO << "\n\n\n";

  for (int k = 0; k < signal.size(); ++k)
  {
    double tmp = static_cast<double>(k) / Fs;

    // signal[i] *= std::exp(std::complex<double>(0.0, -2.0 * M_PI * CFO * static_cast<double>(tmp)));
    signal[k] *= std::exp(-1i * 2.0 * M_PI * CFO * tmp);
  }
}

std::vector<std::complex<double>>
batch_fft(const std::vector<std::complex<double>> &data, int batch_size)
{
  const int N = data.size();

  if (batch_size <= 0 || N % batch_size != 0 || N == 0)
  {
    // printf("INVALID SIZE");
    return {{0, 0}};
  }

  const int howmany = N / batch_size;
  const int n[] = {batch_size};

  fftw_complex *in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
  fftw_complex *out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);

  if (!in || !out)
  {
    printf("FFTW malloc failed!\n");
    if (in)
      fftw_free(in);
    if (out)
      fftw_free(out);
    return {};
  }

  for (int i = 0; i < N; ++i)
  {
    in[i][0] = static_cast<double>(data[i].real());
    in[i][1] = static_cast<double>(data[i].imag());
  }

  fftw_plan plan =
      fftw_plan_many_dft(1, n, howmany, in, nullptr, 1, batch_size, out,
                         nullptr, 1, batch_size, FFTW_FORWARD, FFTW_ESTIMATE);

  if (!plan)
  {
    printf("FFT dont start!");
    fftw_free(in);
    fftw_free(out);
    return {};
  }

  fftw_execute(plan);

  std::vector<std::complex<double>> result(N);

  for (int i = 0; i < N; ++i)
  {
    result[i] = {static_cast<double>(out[i][0]),
                 static_cast<double>((out[i][1]))};
  }

  fftw_destroy_plan(plan);
  fftw_free(in);
  fftw_free(out);

  return result;
}

std::vector<std::complex<double>>
delete_CP(const std::vector<std::complex<double>> &samples,
          const std::vector<int> &peaks, const int CP_size,
          const int FFT_size)
{
  /*check errors*/
  if (samples.size() < FFT_size)
  {
    return {};
  }

  std::vector<std::complex<double>> result;

  /*size without CP*/
  result.reserve(peaks.size() * FFT_size);

  for (int i = 0; i < peaks.size(); ++i)
  {
    int peak = peaks[i];

    /*start iterator of OFDM symbol*/
    auto start = samples.begin() + peak + CP_size;

    /*end iterator of OFDM symbol*/
    auto end = samples.begin() + peak + CP_size + FFT_size;

    result.insert(result.end(), start, end);
  }

  return result;
}

std::vector<std::complex<double>> ofdm_sym_template(std::complex<double> pilot,
                                                    const int FFT_size,
                                                    const int pilots_count,
                                                    const int gi_size)
{

  std::vector<std::complex<double>> temp(FFT_size);

  int payload_size = FFT_size - 2 * gi_size;

  int step = payload_size / pilots_count;

  for (int i = gi_size - 1; i < FFT_size - gi_size; i += step)
  {
    temp[i] = pilot;
  }

  return temp;
}

std::vector<std::complex<double>>
create_ofdm_symbols(const std::vector<std::complex<double>> &symbols,
                    const int Nc, const int gi_size)
{
  std::vector<std::complex<double>> ofdm_symbols(symbols.size() + gi_size * 2);

  for (int i = gi_size; i < symbols.size(); ++i)
  {
    ofdm_symbols[i] = symbols[i - gi_size];
  }
}

std::vector<cell_type> create_ofdm_grid(const int FFT_size,
                                        const int pilots_count,
                                        const int gi_size)
{
  std::vector<cell_type> grid(FFT_size, data);

  // --- 1. Guard bands ---
  for (int i = 0; i < gi_size; ++i)
  {
    grid[i] = guard;
    grid[FFT_size - 1 - i] = guard;
  }

  // --- 2. DC (k = 0) ---
  grid[0] = guard; // или отдельный тип, но логически это "не используется"

  // --- 3. Определяем usable диапазон ---
  int start = gi_size;              // первый после guard
  int end = FFT_size - gi_size - 1; // последний перед правым guard

  // исключаем DC, если он попал в диапазон
  if (start == 0)
    start = 1;

  int usable = end - start + 1;

  // --- 4. Расстановка пилотов ---
  for (int i = 0; i < pilots_count; ++i)
  {
    // равномерно по usable области
    int pos = start + (i * usable) / pilots_count;

    // защита от попадания в DC
    if (pos == 0)
      continue;

    grid[pos] = pilot;
  }

  return grid;
}

// std::vector<cell_type> create_ofdm_grid(const int FFT_size,
//                                         const int pilots_count,
//                                         const int gi_size)
// {
//   // create grid and fill her by data cell
//   std::vector<cell_type> grid(FFT_size, data);

//   // fill left/right guard
//   for (int i = 0; i < gi_size; ++i)
//   {
//     grid[i] = guard;
//     grid[grid.size() - i - 1] = guard;
//   }

//   /*
//     compute space b/w pilots. Each symbol has pilots on the sides, and in the
//     center the pilots are distributed evenly
//    */
//   double pilot_step = double(FFT_size - 2 * gi_size - 1) / (pilots_count - 1);

//   int pilot_pos;

//   // fill pilots
//   for (int i = 0; i < pilots_count; ++i)
//   {
//     pilot_pos = gi_size + std::lround(i * pilot_step);
//     grid[pilot_pos] = pilot;
//   }

//   return grid;
// }

std::vector<std::complex<double>>
create_ofdm_signal(const std::vector<std::complex<double>> &symbols,
                   const std::vector<cell_type> &grid,
                   const std::complex<double> pilot_value,
                   const int buff_size)
{
  /*buff size - size of buffer in SDR*/
  std::vector<std::complex<double>> signal;

  int k = 0;

  while (k < symbols.size())
  {
    for (int j = 0; j < grid.size(); ++j)
    {

      /*if cell is guard, then signal is (0,0)*/
      if (grid[j] == guard)
      {
        signal.push_back({0, 0});
      }

      /*if cell is pilot, then insert pilot*/
      else if (grid[j] == pilot)
      {
        signal.push_back(pilot_value);
      }

      /*if cell is data, then insert symbol*/
      else if (grid[j] == data)
      {
        if (k >= symbols.size())
        {
          signal.push_back({0, 0});
        }
        else
        {
          signal.push_back(symbols[k++]);
        }
      }
    }
  }

  return signal;
}

std::vector<int> get_pilots_pos(const std::vector<cell_type> &grid)
{

  std::vector<int> pos;

  for (int i = 0; i < grid.size(); ++i)
  {
    if (grid[i] == pilot)
    {
      pos.push_back(i);
    }
  }

  return pos;
}

void linear_interpolation(std::vector<std::complex<double>> &H,
                          const std::vector<int> &pos, int FFT_size)
{
  if (pos.size() < 2)
    return;

  int sym = 0;
  bool flag = true;

  while (flag)
  {
    for (int i = 0; i < pos.size() - 1; ++i)
    {
      int left_index = pos[i] + sym * FFT_size;
      int right_index = pos[i + 1] + sym * FFT_size;

      if (left_index >= H.size() || right_index >= H.size())
      {
        flag = false;
        break;
      }

      std::complex<double> left_point = H[left_index];
      std::complex<double> right_point = H[right_index];

      for (int idx = left_index + 1; idx < right_index; ++idx)
      {
        double coeff = static_cast<double>(idx - left_index) /
                       static_cast<double>(right_index - left_index);

        H[idx] = left_point + coeff * (right_point - left_point);
      }
    }

    ++sym;
  }
}

void linear_interpolation2(std::vector<double> &H, const std::vector<int> &pos,
                           int FFT_size)
{
  if (pos.size() < 2)
    return;

  int sym = 0;
  bool flag = true;

  while (flag)
  {
    for (int i = 0; i < pos.size() - 1; ++i)
    {
      int left_index = pos[i] + sym * FFT_size;
      int right_index = pos[i + 1] + sym * FFT_size;

      if (left_index >= H.size() || right_index >= H.size())
      {
        flag = false;
        break;
      }

      double left_point = H[left_index];
      double right_point = H[right_index];

      for (int idx = left_index + 1; idx < right_index; ++idx)
      {
        double coeff = static_cast<double>(idx - left_index) /
                       static_cast<double>(right_index - left_index);

        H[idx] = left_point + coeff * (right_point - left_point);
      }
    }

    ++sym;
  }
}

// void unwrap_phase(std::vector<double> &phase, int FFT_size)
// {
//   int total_size = phase.size();
//   int symbs_count = total_size / FFT_size;

//   for (int s = 0; s < symbs_count; ++s)
//   {
//     double offset = 0.0;
//     int prev_idx = -1;

//     int start = s * FFT_size;
//     int end = start + FFT_size;

//     for (int i = start; i < end; ++i)
//     {
//       if (phase[i] == 0.0)
//         continue;

//       if (prev_idx == -1)
//       {
//         prev_idx = i;
//         continue;
//       }

//       double d = phase[i] - phase[prev_idx];

//       if (d > M_PI)
//         offset -= 2 * M_PI;
//       else if (d < -M_PI)
//         offset += 2 * M_PI;

//       phase[i] += offset;

//       prev_idx = i;
//     }
//   }
// }

void unwrap_phase(std::vector<double> &phase,
                  const std::vector<int> &pilots_pos,
                  int sym, int FFT_size)
{
  double offset = 0.0;

  int base = sym * FFT_size;

  double prev = phase[base + pilots_pos[0]];

  for (int i = 1; i < pilots_pos.size(); ++i)
  {
    int idx = base + pilots_pos[i];

    double curr = phase[idx];
    double d = curr - prev;

    if (d > M_PI)
      offset -= 2 * M_PI;
    else if (d < -M_PI)
      offset += 2 * M_PI;

    curr += offset;
    phase[idx] = curr;

    prev = curr; // ВАЖНО: уже unwrap-нутое значение
  }
}

std::vector<std::complex<double>>
channel_estimation(std::vector<std::complex<double>> &signal,
                   const std::vector<cell_type> &grid,
                   std::complex<double> pilot_value, rx_cfg &rx_config)
{
  const std::vector<int> pilots_pos = get_pilots_pos(grid);
  const int FFT_size = grid.size();
  const int symbs_count = signal.size() / FFT_size;

  std::vector<std::complex<double>> estimation(signal.size());

  for (int i = 0; i < symbs_count; ++i)
  {
    std::complex<double> sum(0.0, 0.0);

    for (int j = 0; j < static_cast<int>(pilots_pos.size()); ++j)
    {
      sum += signal[pilots_pos[j] + i * FFT_size] / pilot_value;
    }

    std::vector<double> A_sym(FFT_size, 0.0);
    std::vector<double> phi_sym(FFT_size, 0.0);

    for (int j = 0; j < static_cast<int>(pilots_pos.size()); ++j)
    {
      const int pos = pilots_pos[j];
      const std::complex<double> h = signal[i * FFT_size + pos] / pilot_value;

      A_sym[pos] = std::abs(h);
      phi_sym[pos] = std::arg(h);
    }

    unwrap_phase(phi_sym, pilots_pos, symbs_count, FFT_size);
    linear_interpolation2(A_sym, pilots_pos, FFT_size);
    linear_interpolation2(phi_sym, pilots_pos, FFT_size);

    for (int k = 0; k < FFT_size; ++k)
    {
      estimation[i * FFT_size + k] =
          A_sym[k] * std::exp(std::complex<double>(0.0, phi_sym[k]));
    }
  }

  return estimation;
}

void channel_equalization(std::vector<std::complex<double>> &symbols,
                          const std::vector<std::complex<double>> &estimation)
{
  for (int i = 0; i < symbols.size(); ++i)
  {
    if (std::abs(estimation[i]) > 1e-12)
      symbols[i] /= estimation[i];
  }
}

void CPE_equalization(std::vector<std::complex<double>> &signal,
                      const std::vector<cell_type> &grid,
                      std::complex<double> pilot_value)
{
  const std::vector<int> pilots_pos = get_pilots_pos(grid);
  const int FFT_size = grid.size();
  const int symbs_count = signal.size() / FFT_size;

  for (int sym = 0; sym < symbs_count; ++sym)
  {
    std::complex<double> sum(0.0, 0.0);

    // --- оценка CPE ---
    for (int p : pilots_pos)
    {
      int idx = sym * FFT_size + p;
      sum += signal[idx] / pilot_value;
    }

    if (std::abs(sum) < 1e-12)
      continue;

    double cpe = std::arg(sum);

    // --- компенсация ---
    std::complex<double> rot = std::exp(std::complex<double>(0.0, -cpe));

    int base = sym * FFT_size;

    for (int k = 0; k < FFT_size; ++k)
    {
      signal[base + k] *= rot;
    }
  }
}

std::vector<std::complex<double>>
extract_symbols(const std::vector<std::complex<double>> &ofdm_symbols,
                const std::vector<cell_type> &grid)
{
  std::vector<std::complex<double>> clear_symbols;
  int k = 0;
  bool flag = true;

  while (flag)
  {
    for (int i = 0; i < grid.size(); ++i)
    {
      if (i + k * grid.size() >= ofdm_symbols.size())
      {
        flag = false;
        break;
      }

      if (grid[i] == data)
      {
        clear_symbols.push_back(ofdm_symbols[i + k * grid.size()]);
      }
    }

    k++;
  }

  return clear_symbols;
}

std::vector<frame_cell> create_frame_grid(const bool pss, const bool symbols)
{

  std::vector<frame_cell> frame_grid;

  if (pss)
    frame_grid.push_back(PSS);

  if (symbol)
    frame_grid.push_back(symbol);

  return frame_grid;
}

std::vector<std::complex<double>> create_frames(const std::vector<frame_cell> &frame_grid, const std::vector<std::complex<double>> pss, const std::vector<std::complex<double>> symbols, const int frames_count, const int pss_size, const int symb_per_frame, const int symb_size)
{

  std::vector<std::complex<double>> frames;
  frames.reserve(frames_count * ((symb_per_frame * symb_size) + pss_size));

  auto start_pss = pss.begin();
  auto end_pss = pss.begin() + pss_size;

  int i = 0;

  for (int i = 0; i < frames_count; ++i)
  {
    for (int j = 0; j < frame_grid.size(); ++j)
    {

      if (frame_grid[i] == PSS)
      {
        frames.insert(frames.end(), start_pss, end_pss);
      }

      if (frame_grid[i] == symbol)
      {
        auto start_symb = symbols.begin() + i * symb_size;
        auto end_symb = symbols.begin() + (i + 1) * symb_size;

        frames.insert(frames.end(), start_symb, end_symb);
      }
    }
  }

  return frames;
}

std::vector<std::complex<double>> ZC_gen(int root, int Nzc)
{
  std::vector<std::complex<double>> d_u(Nzc);

  const std::complex<double> j(0, 1);

  for (int n = 1; n < Nzc; ++n)
  {
    double nd = static_cast<double>(n);
    double ud = static_cast<double>(root);

    if (Nzc % 2 == 0)
    {
      d_u[n] = std::exp(-j * M_PI * ud * nd * nd / static_cast<double>(Nzc));
    }
    else
    {
      d_u[n] = std::exp(-j * M_PI * ud * nd * (nd + 1) / static_cast<double>(Nzc));
    }
  }

  return d_u;
}

std::vector<std::complex<double>> add_ZC(const std::vector<std::complex<double>> &ofdm_samples, const std::vector<std::complex<double>> &ZC)
{
  std::vector<std::complex<double>> result;
  result.reserve(ofdm_samples.size() + 2 * ZC.size());

  result.insert(result.end(), ZC.begin(), ZC.end());
  result.insert(result.end(), ofdm_samples.begin(), ofdm_samples.end());
  result.insert(result.end(), ZC.begin(), ZC.end());

  return result;
}

std::vector<double> ZC_corr(const std::vector<std::complex<double>> &samples,
                            const std::vector<std::complex<double>> &ZC)
{
  std::vector<double> corr_func;

  corr_func.reserve(samples.size() - ZC.size());

  double B = 0;

  std::complex<double> R = 0;
  double A = 0;
  double coeff = std::sqrt(A * B);

  for (int i = 0; i < ZC.size(); ++i)
    B += std::norm(ZC[i]);

  for (int k = 0; k <= samples.size() - ZC.size(); ++k)
  {
    R = 0;
    A = 0;

    for (int i = 0; i < ZC.size(); ++i)
    {
      R += samples[k + i] * std::conj(ZC[i]);
      A += std::norm(samples[k + i]);
    }

    coeff = std::sqrt(A * B);
    corr_func.push_back(coeff != 0.0 ? std::abs(R) / coeff : 0);
  }

  return corr_func;
}

std::vector<std::complex<double>>
extract_inner_symbols(const std::vector<std::complex<double>> &ofdm_symbols,
                      const std::vector<cell_type> &grid)
{
  std::vector<std::complex<double>> clear_symbols;
  int k = 0;
  bool flag = true;

  while (flag)
  {
    for (int i = 0; i < grid.size(); ++i)
    {
      /*check out of range*/
      if (i + k * grid.size() >= ofdm_symbols.size())
      {
        flag = false;
        break;
      }

      /*extract symbols with data only*/
      if (grid[i] == data && std::abs(ofdm_symbols[i + k * grid.size()]) > 1e-12)
      {
        clear_symbols.push_back(ofdm_symbols[i + k * grid.size()]);
      }
    }

    k++;
  }
  // clear_symbols.resize(clear_symbols.size() - padding);
  return clear_symbols;
}

void PSS_CFO_CORRECTION(std::vector<std::complex<double>> &samples, const std::vector<int> &peaks, const std::vector<std::complex<double>> &ZC, const double Ts)
{
  int peak;
  int range = peaks[1] - peaks[0];

  std::vector<std::complex<double>> corr;
  std::complex<double> tmp_corr;
  for (int i = 0; i < peaks.size(); ++i)
  {
    peak = peaks[i];
    tmp_corr = 0;

    for (int j = 0; j < ZC.size(); ++j)
    {
      tmp_corr += samples[peak + j] * std::conj(ZC[j]);
    }

    corr.push_back(tmp_corr);
  }

  const double phase = std::arg(corr[1] * std::conj(corr[0]));
  const double cfo = -phase / (2.0 * M_PI * Ts * static_cast<double>(range));

  for (int i = 0; i < static_cast<int>(samples.size()); ++i)
  {
    const double phi = -2.0 * M_PI * cfo * Ts * static_cast<double>(i);
    samples[i] *= std::exp(std::complex<double>(0.0, phi));
  }

  std::cout << "Ts = " << Ts << "\n";
  std::cout << "range = " << range << "\n";
  std::cout << "phase = " << phase << "\n";
  std::cout << "cfo = " << cfo << "\n";
}

double coarse_cfo(std::vector<std::complex<double>> &r, int max_index, int FFT_size, int Lcp, double fs)
{
  std::complex<double> P = 0.0f;
  for (int i = 0; i < Lcp; ++i)
    P += r[max_index + i] * std::conj(r[max_index + i + FFT_size]);

  double epsilon = std::arg(P) / (2 * M_PIf);

  double cfo_hz = epsilon * fs / FFT_size;

  for (size_t n = 0; n < r.size(); ++n)
  {
    double phase = 2 * M_PIf * cfo_hz * n / fs;
    r[n] *= std::complex<double>(std::cos(phase), std::sin(phase));
  }

  return cfo_hz;
}

void fft_shift_ofdm_symbols(std::vector<std::complex<double>> &samples, int FFT_size)
{
  int ofdm_symbols = samples.size() / FFT_size;

  for (int i = 0; i < ofdm_symbols; ++i)
  {
    auto start = samples.begin() + i * FFT_size;
    auto mid = start + FFT_size / 2;
    auto end = start + FFT_size;

    std::rotate(start, mid, end);
  }
}