#include <complex>
#include <vector>
// #include <spdlog/spdlog.h>
#include "../includes/find_peaks.hpp"
#include <algorithm>
#include <cstring>
#include <fftw3.h>
#include <iostream>
#include <random>
#include "../src/RX/fft.hpp"

struct tx_cfg
{
  int count_OFDM_symb;
  int CP_size;
  int Nc;
};

enum cell_type
{
  guard,
  data,
  pilot
};
enum frame_cell
{
  PSS,
  symbol
};

// std::vector<std::vector<std::complex<double>>> batched(const
// std::vector<std::complex<double>> &data, const int size)
// {
//     if (data.size() % size != 0)
//     {
//         printf("ERROR!\n");
//         return {};
//     }

//     const int batch_count = data.size() / size;
//     std::vector<std::vector<std::complex<double>>> batches;
//     batches.reserve(batch_count);

//     for (int i = 0; i < batch_count; ++i)
//     {
//         auto start = data.cbegin() + (i * size);
//         auto end = data.begin() + ((i + 1) * size);

//         batches.emplace_back(std::vector<std::complex<double>>(start, end));
//     }

//     return batches;
// }

std::vector<std::vector<std::complex<double>>>
batched(const std::vector<std::complex<double>> &data, const int size)
{
  // if (data.size() % size != 0)
  // {
  //     spdlog::error("[OFDM.cpp]: data size % batch size != 0!");
  //     return {};
  // }

  const int batch_count = data.size() / size;
  std::vector<std::vector<std::complex<double>>> batches;
  batches.reserve(batch_count);

  for (int i = 0; i < batch_count; ++i)
  {
    auto start = data.cbegin() + (i * size);
    auto end = data.begin() + ((i + 1) * size);

    batches.emplace_back(std::vector<std::complex<double>>(start, end));
  }

  return batches;
}

std::vector<std::complex<double>>
batch_ifft(const std::vector<std::complex<double>> &data, int batch_size)
{
  const int N = data.size();

  if (batch_size <= 0 || N % batch_size != 0)
    return {};

  const int howmany = N / batch_size;
  const int n[] = {batch_size};

  fftw_complex *in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
  fftw_complex *out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);

  std::memcpy(in, data.data(), sizeof(fftw_complex) * N);

  fftw_plan plan =
      fftw_plan_many_dft(1, n, howmany, in, nullptr, 1, batch_size, out,
                         nullptr, 1, batch_size, FFTW_BACKWARD, FFTW_ESTIMATE);

  if (!plan)
  {
    fftw_free(in);
    fftw_free(out);
    return {};
  }

  fftw_execute(plan);

  std::vector<std::complex<double>> result(N);

  for (int i = 0; i < N; ++i)
  {
    result[i] = {out[i][0], out[i][1]};
    result[i] /= batch_size;
  }

  fftw_destroy_plan(plan);
  fftw_free(in);
  fftw_free(out);

  return result;
}

std::vector<std::complex<double>>
add_CP(const std::vector<std::complex<double>> &samples,
       const tx_cfg &tx_config, const int buff_size)
{
  std::vector<std::complex<double>> result(
      samples.size() + tx_config.count_OFDM_symb * tx_config.CP_size);

  for (int i = 0; i < tx_config.count_OFDM_symb; ++i)
  {
    auto dst = result.data() + i * (tx_config.CP_size + tx_config.Nc);
    auto src = samples.data() + i * tx_config.Nc;

    std::memcpy(dst, src + (tx_config.Nc - tx_config.CP_size),
                tx_config.CP_size * sizeof(std::complex<double>));

    std::memcpy(dst + tx_config.CP_size, src,
                tx_config.Nc * sizeof(std::complex<double>));
  }

  if (result.size() != buff_size)
  {
    const int padding = buff_size - result.size();
    result.insert(result.end(), padding, {0, 0});
  }

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

std::vector<int16_t> bits_gen(const int N)
{
  std::vector<int16_t> bits;
  bits.resize(N);

  for (int i = 0; i < N; ++i)
  {
    bits[i] = rand() % 2;
  }

  return bits;
}

std::complex<double>
corr(const std::vector<std::complex<double>> &symbols,
     const std::vector<std::complex<double>> &sync_seq_symb)
{
  //   if (symbols.size() != sync_seq_symb.size())
  //   {
  //     spdlog::error("Vectors must be the same size");
  //     return {0.0, 0.0};
  //   }

  std::complex<double> sum{0.0, 0.0};

  for (int i = 0; i < symbols.size(); ++i)
  {
    // std::complex<double> x{static_cast<double>(symbols[i].real()),
    //                        static_cast<double>(symbols[i].imag())};
    // std::complex<double> y{static_cast<double>(sync_seq_symb[i].real()),
    //                        static_cast<double>(sync_seq_symb[i].imag())};

    sum += symbols[i] * std::conj(sync_seq_symb[i]);
  }

  return sum;
}

double norm_corr(const std::vector<std::complex<double>> &symbols,
                 const std::vector<std::complex<double>> &sync_seq_symb)
{
  std::complex<double> unnormal_cor = corr(symbols, sync_seq_symb);

  double norm_coeff_a = 0.0;
  double norm_coeff_b = 0.0;

  for (int i = 0; i < symbols.size(); ++i)
  {
    norm_coeff_a += std::norm(symbols[i]);
    norm_coeff_b += std::norm(sync_seq_symb[i]);
  }

  double norm_coeff = std::sqrt(norm_coeff_a) * std::sqrt(norm_coeff_b);

  if (norm_coeff < 1e-12)
    return 0.0;

  return std::abs(unnormal_cor / norm_coeff);
}

std::vector<double>
OFDM_corr_receive(const std::vector<std::complex<double>> &samples,
                  const int symb_size, const int CP_size)
{
  std::vector<double> corr_function;

  for (int i = 0; i < samples.size() - symb_size - CP_size + 1; ++i)
  {
    std::vector<std::complex<double>> window1(samples.begin() + i,
                                              samples.begin() + i + CP_size);
    std::vector<std::complex<double>> window2(samples.begin() + i + symb_size,
                                              samples.begin() + i + symb_size +
                                                  CP_size);
    corr_function.push_back(norm_corr(window1, window2));
  }

  return corr_function;
}

std::vector<std::complex<double>>
batch_fft(const std::vector<std::complex<double>> &data, int batch_size)
{
  const int N = data.size();

  if (batch_size <= 0 || N % batch_size != 0)
  {
    printf("INVALID SIZE");
    return {};
  }

  const int howmany = N / batch_size;
  const int n[] = {batch_size};

  fftw_complex *in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
  fftw_complex *out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);

  for (int i = 0; i < N; ++i)
  {
    in[i][0] = data[i].real();
    in[i][1] = data[i].imag();
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
    result[i] = {out[i][0], out[i][1]};
  }

  fftw_destroy_plan(plan);
  fftw_free(in);
  fftw_free(out);

  return result;
}

// std::vector<int> find_peaks(const std::vector<double> &corr_func, const
// double threshold)
// {
//     double cur;
//     double prev;
//     double next;

//     std::vector<int> index;
//     index.push_back(0);
//     for (int i = 1; i < corr_func.size() - 1; ++i)
//     {
//         cur = corr_func[i];
//         prev = corr_func[i - 1];
//         next = corr_func[i + 1];

//         if (cur > prev && cur > next && cur >= threshold)
//         {
//             index.push_back(i);
//         }
//     }

//     index.push_back(corr_func.size() - 1);

//     return index;
// }

std::vector<std::complex<double>>
del_guard(const std::vector<std::complex<double>> &symbols)
{
  std::vector<std::complex<double>> wo_guard;

  for (int i = 0; i < symbols.size(); ++i)
  {
    if (std::abs(symbols[i]) > 1e-9)
    {
      wo_guard.push_back(symbols[i]);
    }
  }

  return wo_guard;
}

std::vector<std::complex<double>>
extract_OFDM_symbols(const std::vector<std::complex<double>> &ofdm_samples,
                     const std::vector<int> &peaks, const int CP_size,
                     const int Nc)
{
  std::vector<std::complex<double>> result;
  result.reserve(peaks.size() * Nc);

  for (int i = 0; i < peaks.size(); ++i)
  {
    int peak = peaks[i];
    auto start = ofdm_samples.begin() + peak + CP_size;
    auto end = ofdm_samples.begin() + peak + CP_size + Nc;

    result.insert(result.end(), start, end);
  }

  return result;
}

std::vector<int> order_gen(const int N, const int seed)
{
  std::vector<int> order(N);

  for (int i = 0; i < N; ++i)
  {
    order[i] = i;
  }

  std::mt19937 gen(seed);

  std::shuffle(order.begin(), order.end(), gen);

  return order;
}

std::vector<int16_t> intervale(const std::vector<int16_t> &bits,
                               const int seed)
{
  //   if (bits.size() <= 1) {
  //     spdlog::error("[intervaler.cpp]: Bits size must be more then 1!");
  //     return {};
  //   }

  // generate new order
  std::vector<int> order = order_gen(bits.size(), seed);

  std::vector<int16_t> result;
  result.resize(bits.size());

  // shuffle
  for (int i = 0; i < bits.size(); ++i)
  {
    result[order[i]] = bits[i];
  }

  return result;
}

std::vector<int> order_gen_rx(const int N, const int seed)
{
  std::vector<int> order(N);

  for (int i = 0; i < N; ++i)
  {
    order[i] = i;
  }

  std::mt19937 gen(seed);

  std::shuffle(order.begin(), order.end(), gen);

  return order;
}

std::vector<int16_t> deintervale(const std::vector<int16_t> &bits,
                                 const int seed)
{
  //   if (bits.size() <= 1) {
  //     spdlog::error("[deintervaler.cpp]: Bits size must be more then 1!");
  //     return {};
  //   }

  // generate new order
  std::vector<int> order = order_gen_rx(bits.size(), seed);

  std::vector<int16_t> result;
  result.resize(bits.size());

  // shuffle
  for (int i = 0; i < bits.size(); ++i)
  {
    result[i] = bits[order[i]];
  }

  return result;
}

std::vector<cell_type> create_ofdm_grid(const int FFT_size,
                                        const int pilots_count,
                                        const int gi_size)
{
  // create grid and fill her by zeros
  std::vector<cell_type> grid(FFT_size, data);

  // fill left guard
  for (int i = 0; i < gi_size; ++i)
  {
    grid[i] = guard;
  }

  // fill right guard
  for (int i = FFT_size - gi_size - 1; i < FFT_size; ++i)
  {
    grid[i] = guard;
  }

  double pilot_step = double(FFT_size - 2 * gi_size - 1) / (pilots_count - 1);
  int pilot_pos;

  // fill pilots
  for (int i = 0; i < pilots_count; ++i)
  {
    pilot_pos = gi_size + std::lround(i * pilot_step);
    grid[pilot_pos] = pilot;
  }

  return grid;
}

std::vector<std::complex<double>>
create_ofdm_signal(const std::vector<std::complex<double>> &symbols,
                   const std::vector<cell_type> &grid,
                   const std::complex<double> pilot_value,
                   const int buff_size)
{
  std::vector<std::complex<double>> signal;
  signal.reserve(buff_size);

  int k = 0;

  while (k < symbols.size())
  {
    for (int j = 0; j < grid.size(); ++j)
    {

      if (grid[j] == guard)
      {
        signal.push_back({0, 0});
      }

      else if (grid[j] == pilot)
      {
        signal.push_back(pilot_value);
      }

      else
      {
        signal.push_back(symbols[k++]);
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
    for (size_t i = 0; i + 1 < pos.size(); ++i)
    {
      int left_index = pos[i] + sym * FFT_size;
      int right_index = pos[i + 1] + sym * FFT_size;

      if (left_index >= static_cast<int>(H.size()) ||
          right_index >= static_cast<int>(H.size()))
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

std::vector<std::complex<double>>
channel_estimation(const std::vector<std::complex<double>> &signal,
                   const std::vector<cell_type> &grid,
                   std::complex<double> pilot_value)
{
  std::vector<int> pilots_pos = get_pilots_pos(grid);

  // for (int i = 0; i < pilots_pos.size(); ++i)
  // {
  //     std::cout << pilots_pos[i] << " ";
  // }

  // std::cout << "\n\n\n";

  const int FFT_size = grid.size();

  const int symbs_count = signal.size() / FFT_size;

  std::vector<std::complex<double>> estimation(FFT_size * symbs_count);

  for (int i = 0; i < symbs_count; ++i)
  {
    for (int j = 0; j < pilots_pos.size(); ++j)
    {
      estimation[pilots_pos[j] + i * FFT_size] =
          signal[pilots_pos[j] + i * FFT_size] / pilot_value;
    }
  }

  linear_interpolation(estimation, pilots_pos, FFT_size);

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

std::vector<std::complex<double>> add_ZC(const std::vector<std::complex<double>> &ofdm_samples, const std::vector<std::complex<double>> &ZC)
{
  std::vector<std::complex<double>> result;
  result.reserve(ofdm_samples.size() + 2 * ZC.size());

  result.insert(result.end(), ZC.begin(), ZC.end());
  result.insert(result.end(), ofdm_samples.begin(), ofdm_samples.end());
  result.insert(result.end(), ZC.begin(), ZC.end());

  return result;
}

std::vector<std::complex<double>> ZC_gen(const int root)
{

  std::vector<std::complex<double>> d_u;
  d_u.reserve(61);

  const std::complex<double> j(0, 1);

  for (int n = 0; n < 61; ++n)
  {

    double nd = static_cast<double>(n);
    double ud = static_cast<double>(root);

    std::complex<double> d;

    if (n <= 30)
    {
      d = std::exp(-j * M_PI * ud * nd * (nd + 1) / 63.0);
    }
    else
    {
      d = std::exp(-j * M_PI * ud * (nd + 1) * (nd + 2) / 63.0);
    }

    d_u.push_back(d);
  }

  return d_u;
}

int main()
{
  srand(time(nullptr));

  const int FFT_size = 128;
  const int pilots_count = 8;
  const int guard_size = 4;
  const int CP_size = FFT_size / 8;
  const int bps = 1;

  const int buff_size = 1920;
  const int pss_size = 61;

  const int total_buff_size = buff_size - 2 * pss_size;

  const int symbs_count = total_buff_size / (FFT_size + CP_size);

  const int padding = total_buff_size - (symbs_count * (FFT_size + CP_size));

  const int payload_size = FFT_size - 2 * guard_size - pilots_count;

  const int bits_count = symbs_count * payload_size * bps;

  // std::cout << "\n\n"
  //           << total_buff_size << "\n\n";

  // std::cout << "\n\n"
  //           << symbs_count << "\n\n";

  // std::cout << "\n\n"
  //           << padding << "\n\n";

  // std::cout << "\n\n"
  //           << bits_count << "\n\n";

  tx_cfg config;
  config.count_OFDM_symb = symbs_count;
  config.CP_size = CP_size;
  config.Nc = FFT_size;

  double total_ber = 0;

  std::vector<cell_type> grid = create_ofdm_grid(FFT_size, pilots_count, guard_size);
  const std::complex<double> pilot_value(1, 1);

  std::vector<std::complex<double>> ZC = ZC_gen(25);

  ZC = ifft2(ZC);

  // std::cout << "\n\n"
  //           << ZC.size() << "\n\n";

  for (int i = 0; i < 1; ++i)
  {

    std::vector<int16_t> bits = bits_gen(bits_count);

    // std::cout << bits.size() << "\n\n";

    // std::cout << "\nBits: ";

    // for (int i = 0; i < bits.size(); ++i) {
    //   std::cout << bits[i] << " ";
    // }

    // std::vector<int16_t> shuffle_bits = intervale(bits, seed);

    // std::cout << "\n\nShuffle bits: ";

    // for (int i = 0; i < shuffle_bits.size(); ++i)
    // {
    //     std::cout << shuffle_bits[i] << " ";
    // }

    std::vector<std::complex<double>> symbols = BPSK(bits);

    // std::cout << symbols.size() << "\n\n";

    // std::cout << "\n\nSymbols: ";

    // for (int i = 0; i < symbols.size(); ++i) {
    //   std::cout << symbols[i] << " ";
    // }

    std::vector<std::complex<double>> ofdm_symbols =
        create_ofdm_signal(symbols, grid, pilot_value, buff_size);

    // std::cout << ofdm_symbols.size() << "\n\n";

    // std::cout << "\n\nofdm signal: ";

    // for (int i = 0; i < ofdm_symbols.size(); ++i) {
    //   std::cout << ofdm_symbols[i] << " ";
    // }

    // std::cout << "\n\n"
    //           << ofdm_symbols.size() << "\n\n";

    std::vector<std::complex<double>> OFDM_symbols =
        batch_ifft(ofdm_symbols, FFT_size);

    // std::cout << OFDM_symbols.size();

    // std::cout << OFDM_symbols.size() << "\n\n";

    // std::cout << "\n\n\nOFDM_symbols: ";

    // for (int i = 0; i < OFDM_symbols.size(); ++i) {
    //   std::cout << OFDM_symbols[i] << " ";
    // }

    // std::cout << "\n\n\nOFDM_samples: ";

    // std::cout << OFDM_symbols.size() << "\n";

    std::vector<std::complex<double>> ofdm_samples = add_CP(OFDM_symbols, config, total_buff_size);

    ofdm_samples = add_ZC(ofdm_samples, ZC);

    std::cout << ofdm_samples.size() << "\n\n";

    // for (int i = 0; i < ofdm_samples.size(); ++i) {
    //   std::cout << ofdm_samples[i] << " ";
    // }

    // std::cout << "ZC: " << ZC.size() << "\n\n";

    // for (int i = 0; i < ZC.size(); ++i)
    // {
    //   std::cout << ZC[i] << " ";
    // }

    // std::cout << "ofdm_samples: " << ofdm_samples.size() << "\n\n";

    // for (int i = 0; i < ofdm_samples.size(); ++i)
    // {
    //   std::cout << ofdm_samples[i] << " ";
    // }

    // // std::cout << ofdm_samples.size() << "\n";

    // ofdm_samples.insert(ofdm_samples.begin(), 1, 1);
    // ofdm_samples.insert(ofdm_samples.end(), 1, 1);

    // std::complex<double> rot = std::polar(1.0, M_PI / 6);

    // for (int i = 0; i < ofdm_samples.size(); ++i) {
    //   ofdm_samples[i] *= std::polar(1.0, M_PI / (i + 5));
    // }

    // // std::cout << "\n\n\nCorrelation function:";

    // std::vector<double> corr_func =
    //     OFDM_corr_receive(ofdm_samples, FFT_size, CP_size);

    // for (int i = 0; i < corr_func.size(); ++i) {
    //   std::cout << corr_func[i] << " ";
    // }

    // findPeaks::PeakConditions conditions;
    // conditions.set_height(0.9); // Minimum height of 2.0
    // // conditions.set_prominence(1.0); // Minimum prominence of 1.0
    // conditions.set_distance(FFT_size); // At least 2 samples between peaks
    // // conditions.set_width(1.0, 3.0);       // Width between 1.0 and 3.0
    // // samples conditions.set_rel_height(0.7);       // Measure width at 70% of
    // // peak height

    // std::vector<int> real_peaks =
    //     findPeaks::find_peaks(corr_func, conditions);

    // std::cout << "\n\n\nPeaks index:";

    // for (int i = 0; i < real_peaks.size(); ++i) {
    //   std::cout << real_peaks[i] << " ";
    // }
  }

  return 0;
}

// const int ofdm_symb_size = FFT_size + CP_size;
// const int payload_size = FFT_size - 2 * guard_size - pilots_count;
// const int buff_size = 70;
// const int bps = 1;

// const int ofdm_symbs_count = buff_size / ofdm_symb_size;

// const int N_bits = ofdm_symbs_count * payload_size * bps;

// tx_cfg config;
// config.count_OFDM_symb = ofdm_symbs_count;
// config.CP_size = CP_size;
// config.Nc = FFT_size;

// double total_ber = 0;

// std::vector<cell_type> grid =
//     create_ofdm_grid(FFT_size, pilots_count, guard_size);
// const std::complex<double> pilot_value(1, 1);

// for (int i = 0; i < 1; ++i) {

//   std::vector<int16_t> bits = bits_gen(N_bits);

//   std::cout << "\nBits: ";

//   for (int i = 0; i < bits.size(); ++i) {
//     std::cout << bits[i] << " ";
//   }

//   // std::vector<int16_t> shuffle_bits = intervale(bits, seed);

//   // std::cout << "\n\nShuffle bits: ";

//   // for (int i = 0; i < shuffle_bits.size(); ++i)
//   // {
//   //     std::cout << shuffle_bits[i] << " ";
//   // }

//   std::vector<std::complex<double>> symbols = BPSK(bits);

//   std::cout << "\n\nSymbols: ";

//   for (int i = 0; i < symbols.size(); ++i) {
//     std::cout << symbols[i] << " ";
//   }

//   std::vector<std::complex<double>> ofdm_symbols =
//       create_ofdm_signal(symbols, grid, pilot_value, buff_size);

//   std::cout << "\n\nofdm signal: ";

//   for (int i = 0; i < ofdm_symbols.size(); ++i) {
//     std::cout << ofdm_symbols[i] << " ";
//   }

//   // std::cout << "\n\n"
//   //           << ofdm_symbols.size() << "\n\n";

//   std::vector<std::complex<double>> OFDM_symbols =
//       batch_ifft(ofdm_symbols, FFT_size);

//   std::cout << "\n\n\nOFDM_symbols: ";

//   for (int i = 0; i < OFDM_symbols.size(); ++i) {
//     std::cout << OFDM_symbols[i] << " ";
//   }

//   std::cout << "\n\n\nOFDM_samples: ";

//   // std::cout << OFDM_symbols.size() << "\n";

//   std::vector<std::complex<double>> ofdm_samples =
//       add_CP(OFDM_symbols, config, buff_size);

//   for (int i = 0; i < ofdm_samples.size(); ++i) {
//     std::cout << ofdm_samples[i] << " ";
//   }

//   // std::cout << ofdm_samples.size() << "\n";

//   ofdm_samples.insert(ofdm_samples.begin(), 1, 1);
//   ofdm_samples.insert(ofdm_samples.end(), 1, 1);

//   std::complex<double> rot = std::polar(1.0, M_PI / 6);

//   for (int i = 0; i < ofdm_samples.size(); ++i) {
//     ofdm_samples[i] *= std::polar(1.0, M_PI / (i + 5));
//   }

//   // std::cout << "\n\n\nCorrelation function:";

//   std::vector<double> corr_func =
//       OFDM_corr_receive(ofdm_samples, FFT_size, CP_size);

//   for (int i = 0; i < corr_func.size(); ++i) {
//     std::cout << corr_func[i] << " ";
//   }

//   findPeaks::PeakConditions conditions;
//   conditions.set_height(0.9); // Minimum height of 2.0
//   // conditions.set_prominence(1.0); // Minimum prominence of 1.0
//   conditions.set_distance(FFT_size); // At least 2 samples between peaks
//   // conditions.set_width(1.0, 3.0);       // Width between 1.0 and 3.0
//   // samples conditions.set_rel_height(0.7);       // Measure width at 70% of
//   // peak height

//   std::vector<int> real_peaks =
//       findPeaks::find_peaks(corr_func, conditions);

//   std::cout << "\n\n\nPeaks index:";

//   for (int i = 0; i < real_peaks.size(); ++i) {
//     std::cout << real_peaks[i] << " ";
//   }

//   std::vector<std::complex<double>> rx_symbols =
//       extract_OFDM_symbols(ofdm_samples, real_peaks, CP_size, FFT_size);

//   std::cout << "\n\n\nRx OFDM symbols:";

//   for (int i = 0; i < rx_symbols.size(); ++i) {
//     std::cout << rx_symbols[i] << " ";
//   }

//   std::vector<std::complex<double>> rx_bpsk = batch_fft(rx_symbols, FFT_size);

//   // rx_bpsk = del_guard(rx_bpsk);

//   std::vector<std::complex<double>> ch_estimation =
//       channel_estimation(rx_bpsk, grid, pilot_value);

//   std::cout << "\n\n\nChannel estimation:";

//   for (int i = 0; i < ch_estimation.size(); ++i) {
//     std::cout << ch_estimation[i] << " ";
//   }

//   channel_equalization(rx_bpsk, ch_estimation);

//   std::cout << "\n\n\nRECOVERY_SIGNAL:";

//   for (int i = 0; i < rx_bpsk.size(); ++i) {
//     std::cout << rx_bpsk[i] << " ";
//   }

//   rx_bpsk = extract_symbols(rx_bpsk, grid);

//   std::cout << "\n\n\nRx BPSK symbols:";

//   for (int i = 0; i < rx_bpsk.size(); ++i) {
//     std::cout << rx_bpsk[i] << " ";
//   }

//   std::vector<int16_t> rx_bits;

//   const double eps = 1e-6;

//   for (size_t i = 0; i < rx_bpsk.size(); ++i) {
//     double re = std::real(rx_bpsk[i]);

//     if (std::abs(re) < eps)
//       continue;

//     if (std::abs(re - 1.0) < eps)
//       continue;

//     rx_bits.push_back(re > 0 ? 0 : 1);
//   }

//   // std::cout << "\n\n\nRx bits:";

//   // for (int i = 0; i < rx_bits.size(); ++i)
//   // {
//   //     std::cout << rx_bits[i] << " ";
//   // }

//   if (rx_bits.size() != bits.size()) {
//     printf("\n\nERROR!!!!!!!!!!!!!!!");
//     continue;
//   }

//   double BER = 0;

//   for (int i = 0; i < rx_bits.size(); ++i) {
//     BER += bits[i] == rx_bits[i] ? 0 : 1;
//   }

//   BER /= rx_bits.size();

//   total_ber += BER;
// }

// std::cout << "\nBER: " << total_ber << "\n\n";

//   return 0;
// }