#include <SoapySDR/Device.h>
#include <SoapySDR/Formats.h>
#include <complex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

#include "../../includes/main/pluto_lib.h"

int main(int argc, char *argv[]) {
  // Наши параметры SDR в одной структуре.
  sdr_config_t config;
  config.name = argv[1];
  config.buffer_size = 1920;
  config.rx_carrier_freq = 800e6;
  config.tx_carrier_freq = 800e6;
  config.rx_sample_rate = 1e6;
  config.tx_sample_rate = 1e6;
  config.rx_gain = 30.0;
  config.tx_gain = -40.0;
  std::cout << config.name << std::endl;

  // size_t channels[] = {0} ; // {0} or {0, 1}
  // config.channels = &channels;

  struct SoapySDRDevice *sdr = setup_pluto_sdr(&config);
  struct SoapySDRStream *rxStream = setup_stream(sdr, &config, 1);
  struct SoapySDRStream *txStream = setup_stream(sdr, &config, 0);

  int16_t tx_buffer[2 * config.buffer_size];
  int16_t rx_buffer[2 * config.buffer_size];

  // TODO: здесь нужно отправлять и принимать сэмплы.
  fill_test_tx_buffer(tx_buffer, config.buffer_size);

  start_rx_tx(sdr, rxStream, txStream, tx_buffer, rx_buffer, config.buffer_size,
              10);

  close_pluto_sdr(sdr, rxStream, txStream);

  return EXIT_SUCCESS;
}