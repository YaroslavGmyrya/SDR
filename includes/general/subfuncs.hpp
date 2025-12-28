#include <complex>
#include <vector>

std::vector<int16_t> dec2bin(const int number, const int bit_format);
std::vector<std::complex<double>> downscaling(const std::vector<std::complex<double>>& samples);
std::vector<std::complex<int16_t>> read_pcm(const std::string& filename);
std::vector<std::complex<int16_t>> upscaling(const std::vector<std::complex<double>>& psf_symbols);
void write_pcm(const std::string& filename, std::vector<std::complex<int16_t>> samples);
double bits_to_pam_lvl(int a, int b);
std::vector<int16_t> generate_barker_code(int len);