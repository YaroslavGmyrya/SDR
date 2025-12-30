#include <SoapySDR/Device.h>
#include <SoapySDR/Formats.h>
#include <complex.h>
#include <cstdint>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "../../includes/general/subfuncs.hpp"
#include "../../includes/main/pluto_lib.hpp"

struct SoapySDRDevice *setup_pluto_sdr(sdr_config_t *config) {
  // Создание сущности устройства
  // args can be user defined or from the enumeration result
  SoapySDRKwargs args = {};
  char buffer_size[10]; // Allocate enough space
  sprintf(buffer_size, "%d", config->buffer_size);
  SoapySDRKwargs_set(&args, "driver", "plutosdr");
  if (1) {
    SoapySDRKwargs_set(&args, "uri", config->name);
  } else {
    SoapySDRKwargs_set(&args, "uri", "ip:192.168.2.1");
  }
  SoapySDRKwargs_set(&args, "direct", "1");
  SoapySDRKwargs_set(&args, "timestamp_every", buffer_size);
  SoapySDRKwargs_set(&args, "loopback", "0");
  SoapySDRDevice *sdr = SoapySDRDevice_make(&args);
  SoapySDRKwargs_clear(&args);

  if (sdr == NULL) {
    printf("SoapySDRDevice_make fail: %s\n", SoapySDRDevice_lastError());
    return NULL;
  }

  // Настраиваем устройства RX\TX
  if (SoapySDRDevice_setSampleRate(sdr, SOAPY_SDR_RX, 0,
                                   config->rx_sample_rate) != 0) {
    printf("setSampleRate rx fail: %s\n", SoapySDRDevice_lastError());
  }
  if (SoapySDRDevice_setFrequency(sdr, SOAPY_SDR_RX, 0, config->rx_carrier_freq,
                                  NULL) != 0) {
    printf("setFrequency rx fail: %s\n", SoapySDRDevice_lastError());
  }
  if (SoapySDRDevice_setSampleRate(sdr, SOAPY_SDR_TX, 0,
                                   config->tx_sample_rate) != 0) {
    printf("setSampleRate tx fail: %s\n", SoapySDRDevice_lastError());
  }
  if (SoapySDRDevice_setFrequency(sdr, SOAPY_SDR_TX, 0, config->tx_carrier_freq,
                                  NULL) != 0) {
    printf("setFrequency tx fail: %s\n", SoapySDRDevice_lastError());
  }
  printf("SoapySDRDevice_getFrequency tx: %lf\n",
         SoapySDRDevice_getFrequency(sdr, SOAPY_SDR_TX, 0));

  return sdr;
}

struct SoapySDRStream *setup_stream(struct SoapySDRDevice *sdr,
                                    sdr_config_t *config, bool isRx) {
  size_t channel_count = sizeof(config->channels) / sizeof(config->channels[0]);
  printf("Channel count = %d\n", channel_count);
  SoapySDRStream *stream;

  if (isRx) {
    stream = SoapySDRDevice_setupStream(sdr, SOAPY_SDR_RX, SOAPY_SDR_CS16,
                                        config->channels, channel_count, NULL);
    if (stream == NULL) {
      printf("setupStream rx fail: %s\n", SoapySDRDevice_lastError());
      SoapySDRDevice_unmake(sdr);
      return NULL;
    }
    if (SoapySDRDevice_setGain(sdr, SOAPY_SDR_RX, *config->channels,
                               config->rx_gain) != 0) {
      printf("setGain rx fail: %s\n", SoapySDRDevice_lastError());
    }

    // Получение MTU (Maximum Transmission Unit), в нашем случае - размер
    // буферов.
    size_t rx_mtu = SoapySDRDevice_getStreamMTU(sdr, stream);
    printf("MTU - RX: %lu\n", rx_mtu);

    // activate streams
    SoapySDRDevice_activateStream(sdr, stream, 0, 0, 0); // start streaming

    printf("Streams are activated\n");
  } else {
    stream = SoapySDRDevice_setupStream(sdr, SOAPY_SDR_TX, SOAPY_SDR_CS16,
                                        config->channels, channel_count, NULL);
    if (stream == NULL) {
      printf("setupStream tx fail: %s\n", SoapySDRDevice_lastError());
      SoapySDRDevice_unmake(sdr);
      return NULL;
    }
    if (SoapySDRDevice_setGain(sdr, SOAPY_SDR_TX, *config->channels,
                               config->tx_gain) != 0) {
      printf("setGain tx fail: %s\n", SoapySDRDevice_lastError());
    }
    size_t tx_mtu = SoapySDRDevice_getStreamMTU(sdr, stream);
    printf("MTU - TX: %lu\n", tx_mtu);

    SoapySDRDevice_activateStream(sdr, stream, 0, 0, 0); // start streaming
  }
  return stream;
}

void start_rx(struct SoapySDRDevice *sdr, SoapySDRStream *rxStream,
              int16_t *rx_buffer, int buff_size, char *filename,
              int work_time) {
  printf("RX STARTED!\n");

  const long timeoutUs = 400000; // arbitrarily chosen (взяли из srsRAN)

  /*get current time*/
  time_t start = time(NULL);

  /*open file for write receiving samples*/
  FILE *file = fopen(filename, "wb");

  long long last_time = 0;

  /*sdr receive samples work_time seconds*/
  while (difftime(time(NULL), start) < work_time) {
    void *rx_buffs[] = {rx_buffer};
    int flags;        // flags set by receive operation
    long long timeNs; // timestamp for receive buffer

    // считали буффер RX, записали его в rx_buffer
    int sr = SoapySDRDevice_readStream(sdr, rxStream, rx_buffs, buff_size,
                                       &flags, &timeNs, timeoutUs);

    // Смотрим на количество считаных сэмплов, времени прихода и разницы во
    // времени с чтением прошлого буфера
    printf(
        "Buffer: %lu - Samples: %i, Flags: %i, Time: %lli, TimeDiff: %llins \n",
        i, sr, flags, timeNs, timeNs - last_time);

    last_time = timeNs;

    fwrite(rx_buffer, 2 * rx_mtu * sizeof(int16_t), 1, rx_data);
  }

  fclose(file);
}

void start_tx(struct SoapySDRDevice *sdr, SoapySDRStream *txStream,
              int16_t *tx_samples, int16_t *tx_buffer, int buff_size,
              int work_time, int filename) {
  printf("TX STARTED!\n");

  const long timeoutUs = 400000; // arbitrarily chosen (взяли из srsRAN)

  /*get current time*/
  time_t start = time(NULL);
  long long last_time = 0;

  /*read samples from .pcm file*/
  std::vector<complex<int16_t>> samples = read_pcm(std::string(filename));

  int samples_count = samples.size();
  ;

  while (difftime(time(NULL), start) < work_time) {
    for (size_t offset = 0; offset < sample_count; offset += buff_size * 2) {
      void *rx_buffs[] = {rx_buffer};
      int flags;        // flags set by receive operation
      long long timeNs; // timestamp for receive buffer

      // считали буффер RX, записали его в rx_buffer
      int sr = SoapySDRDevice_readStream(sdr, rxStream, rx_buffs, buff_size,
                                         &flags, &timeNs, timeoutUs);

      last_time = timeNs;

      // Переменная для времени отправки сэмплов относительно текущего приема
      long long tx_time = timeNs + (4 * 1000 * 1000); // на 4 [мс] в будущее

      // Здесь отправляем наш tx_buff массив
      if (offset + buff_size * 2 >= sample_count)
        break;

      void *tx_buffs[] = {samples + offset};
      fwrite(samples + offset, 2 * rx_mtu * sizeof(int16_t), 1, tx_data);
      printf("offset: %d", offset);
      flags = SOAPY_SDR_HAS_TIME;
      int st = SoapySDRDevice_writeStream(sdr, txStream,
                                          (const void *const *)tx_buffs, tx_mtu,
                                          &flags, tx_time, timeoutUs);
      if ((size_t)st != tx_mtu) {
        printf("TX Failed: %in", st);
      }
    }
  }
}

void close_pluto_sdr(SoapySDRDevice *sdr, SoapySDRStream *rxStream,
                     SoapySDRStream *txStream) {
  printf("Trying to close Pluto SDR\n");
  // stop streaming
  if (sdr || rxStream || txStream) {
    SoapySDRDevice_deactivateStream(sdr, rxStream, 0, 0);
    SoapySDRDevice_deactivateStream(sdr, txStream, 0, 0);

    // shutdown the stream
    SoapySDRDevice_closeStream(sdr, rxStream);
    SoapySDRDevice_closeStream(sdr, txStream);

    // cleanup device handle
    SoapySDRDevice_unmake(sdr);
  }

  printf("Pluto SDR is closed. Streams are deactivated.\n");
}