#include <complex>
#include <fstream>
#include <iostream>
#include <vector>

#include "../../../includes/general/subfuncs.hpp"

void write_pcm(const std::string &filename,
               std::vector<std::complex<int16_t>> samples) {
  if (samples.size() == 0) {
    std::cout << "Samples is empty!\n";
    return;
  }

  std::ofstream file(filename, std::ios::binary | std::ios::app);

  if (!file) {
    return;
  }

  file.write(reinterpret_cast<const char *>(samples.data()),
             samples.size() * sizeof(std::complex<int16_t>));

  file.close();

  std::cout << "Samples write in " << filename << ".pcm\n\n";
}