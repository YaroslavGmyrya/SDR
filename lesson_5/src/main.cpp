#include <SoapySDR/Device.h>   // Инициализация устройства
#include <SoapySDR/Formats.h>  // Типы данных, используемых для записи сэмплов
#include <stdio.h>             //printf
#include <stdlib.h>            //free
#include <stdint.h>
#include <complex.h>
#include <string.h>

#define ADC_CAPACITY 12
#define BITS_SHIFT 4
#define TAU 10
#define TAU_ON_ELEMENT 20
#define MESSAGE "Hello My Beuatiful World"
#define PATH_TO_AUDIO"../audio_converter/audio_bin.pcm"

uint8_t* stob(char* str, int* out_bits_count) {
    // get string len
    int len = strlen(str);

    // 1 char = 8 bit 
    *out_bits_count = len * sizeof(char) * 8;
    
    // massive for bits
    uint8_t* bits = (uint8_t*)malloc(*out_bits_count * sizeof(uint8_t));

    // check pointer
    if (bits == NULL){
        return NULL;
    }
        
    char c;

    // iterate on string
    for (int i = 0; i < len; i++) {
        // get char 
        c = str[i];
        // iterate on bits massive
        for (int j = 0; j < 8; j++) {
            // convert char to bits 
            bits[i * 8 + j] = (c >> (7 - j)) & 1;
        }
    }
    
    return bits;
}

int16_t* bits_to_rect_signal(uint8_t* bits, int bits_count, int tx_mtu){

    // allocate memory
    int16_t* tx_buff = (int16_t*)malloc(sizeof(int16_t) * tx_mtu * 2);

    // iterate on bitts
    for (int i = 0; i < bits_count; ++i)
    {   
        // fill tx_buff with samples
        for(int j = i*TAU_ON_ELEMENT; j < i*TAU_ON_ELEMENT + 20 && j < tx_mtu*2; j+=2){
            if(bits[i]){
                tx_buff[j] = 2047 << 4;    // I
                tx_buff[j+1] = -2047 << 4; // Q
            } else{
                tx_buff[j] = 0;      //I
                tx_buff[j+1] = 0;    //Q
            }
        }
    }

    return tx_buff;
}

int16_t* bits_to_triangle_signal(uint8_t* bits, int bits_count, int tx_mtu){

    // allocate memory
    int16_t* tx_buff = (int16_t*)malloc(sizeof(int16_t) * tx_mtu * 2);

    // iterate on bitts
    for (int i = 0; i < bits_count; ++i)
    {   
        // fill tx_buff with samples
        for(int j = i*TAU_ON_ELEMENT; j < i*TAU_ON_ELEMENT + 20 && j < tx_mtu*2; j+=2){
            if(bits[i] && j == i*TAU_ON_ELEMENT + 8){
                tx_buff[j] = 2047 << 4;    // I
                tx_buff[j+1] = -2047 << 4; // Q
            } else{
                tx_buff[j] = 0;      //I
                tx_buff[j+1] = 0;    //Q
            }
        }
    }

    return tx_buff;
}

int16_t* parabola_signal(int tx_mtu){

    // allocate memory
    int16_t* tx_buff = (int16_t*)malloc(sizeof(int16_t) * tx_mtu * 2);

    float coef = -tx_mtu / 10;

    for (int i = 0; i < 2 * tx_mtu; i+=2)
    {
        tx_buff[i] = int16_t(coef * coef);   // I
        tx_buff[i+1] = int16_t(coef * coef); // Q

        coef += 0.1;
    }

    return tx_buff;
}

int16_t *read_pcm(const char *filename, size_t *sample_count)
{
    FILE *file = fopen(filename, "rb");

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    printf("file_size = %ldn", file_size);
    int16_t *samples = (int16_t *)malloc(file_size);

    *sample_count = file_size / sizeof(int16_t);

    size_t sf = fread(samples, sizeof(int16_t), *sample_count, file);

    if (sf == 0){
        printf("file %s empty!", filename);
    }

    fclose(file);

    return samples;
}

int main(){

    //При работе с SoapySDR инициализация устройства выполняется при помощи указателя на структуру SoapySDRDevic
    //Аргументы (в рамках библиотеки SoapySDR) имеют формат ключ :  значение :
    SoapySDRKwargs args = {};
    SoapySDRKwargs_set(&args, "driver", "plutosdr");        // Говорим какой Тип устройства 
    if (1) {
        SoapySDRKwargs_set(&args, "uri", "usb:");           // Способ обмена сэмплами (USB)
    } else {
        SoapySDRKwargs_set(&args, "uri", "ip:192.168.3.10"); // Или по IP-адресу
    }
    SoapySDRKwargs_set(&args, "direct", "1");               // 
    SoapySDRKwargs_set(&args, "timestamp_every", "1920");   // Размер буфера + временные метки
    SoapySDRKwargs_set(&args, "loopback", "0");             // Используем антенны или нет
    SoapySDRDevice *sdr = SoapySDRDevice_make(&args);       // Инициализация
    SoapySDRKwargs_clear(&args);


    //Настройка параметров устройств TXRX:

    int sample_rate = 1e6;
    int carrier_freq = 800e6;
    // Параметры RX части
    SoapySDRDevice_setSampleRate(sdr, SOAPY_SDR_RX, 0, sample_rate);
    SoapySDRDevice_setFrequency(sdr, SOAPY_SDR_RX, 0, carrier_freq , NULL);

    // Параметры TX части
    SoapySDRDevice_setSampleRate(sdr, SOAPY_SDR_TX, 0, sample_rate);
    SoapySDRDevice_setFrequency(sdr, SOAPY_SDR_TX, 0, carrier_freq , NULL);

    // Инициализация количества каналов RXTX (в AdalmPluto он один, нулевой)
    size_t channels[] = {0};
    // Настройки усилителей на RXTX
    SoapySDRDevice_setGain(sdr, SOAPY_SDR_RX, channels[0], 65.0); // Чувствительность приемника
    SoapySDRDevice_setGain(sdr, SOAPY_SDR_TX, channels[0], -30.0);// Усиление передатчика

    //Инициализация потоков (stream) для передачи и примема сэмплов:

    int channel_count = std::size(channels);

    // Формирование потоков для передачи и приема сэмплов
    SoapySDRStream *rxStream = SoapySDRDevice_setupStream(sdr, SOAPY_SDR_RX, SOAPY_SDR_CS16, channels, channel_count, NULL);
    SoapySDRStream *txStream = SoapySDRDevice_setupStream(sdr, SOAPY_SDR_TX, SOAPY_SDR_CS16, channels, channel_count, NULL);

    SoapySDRDevice_activateStream(sdr, rxStream, 0, 0, 0); //start streaming
    SoapySDRDevice_activateStream(sdr, txStream, 0, 0, 0); //start streaming

    //Выделение памяти под буфферы (для хранения сэмплов TX, RX):
    size_t rx_mtu = SoapySDRDevice_getStreamMTU(sdr, rxStream);
    size_t tx_mtu = SoapySDRDevice_getStreamMTU(sdr, txStream);

    size_t sample_count = 0;
    int16_t *samples = read_pcm(PATH_TO_AUDIO, &sample_count);

    printf("Size: %d\n\n", sample_count);

    // Выделяем память под буферы RX и TX
    int16_t tx_buff[2 *tx_mtu];
    int16_t rx_buffer[2 *rx_mtu];

    FILE *tx_data = fopen("txdata1.pcm", "w");
    FILE *rx_data = fopen("rxdata1.pcm", "w");

    long long timeoutUs = 400000;
    long long last_time = 0;

    int buffers_read = 1;
    // Начинается работа с получением и отправкой сэмплов
    for (size_t offset = 0; offset < sample_count; offset += 1920 * 2)
    {
        void *rx_buffs[] = {rx_buffer};
        int flags;        // flags set by receive operation
        long long timeNs; //timestamp for receive buffer
        
        // считали буффер RX, записали его в rx_buffer
        int sr = SoapySDRDevice_readStream(sdr, rxStream, rx_buffs, rx_mtu, &flags, &timeNs, timeoutUs);

        // Смотрим на количество считаных сэмплов, времени прихода и разницы во времени с чтением прошлого буфера
        printf("Buffer: %lu - Samples: %i, Flags: %i, Time: %lli, TimeDiff: %llins \n", buffers_read++, sr, flags, timeNs, timeNs - last_time);

        last_time = timeNs;

        fwrite(rx_buffer, 2 * rx_mtu * sizeof(int16_t), 1, rx_data);

        // Переменная для времени отправки сэмплов относительно текущего приема
        long long tx_time = timeNs + (4 * 1000 * 1000); // на 4 [мс] в будущее

        // Добавляем время, когда нужно передать блок tx_buff, через tx_time -наносекунд
        // for(size_t i = 0; i < 8; i++)
        // {
        //     uint8_t tx_time_byte = (tx_time >> (i * 8)) & 0xff;
        //     tx_buff[2 + i] = tx_time_byte << 4;
        // }

        // Здесь отправляем наш tx_buff массив
        if(offset + 1920 * 2 >= sample_count)
            break;

        void *tx_buffs[] = {samples + offset};
        fwrite(samples + offset, 2 * rx_mtu * sizeof(int16_t), 1, tx_data);
        printf("offset: %d", offset);
        flags = SOAPY_SDR_HAS_TIME;
        int st = SoapySDRDevice_writeStream(sdr, txStream, (const void * const*)tx_buffs, tx_mtu, &flags, tx_time, timeoutUs);
        if ((size_t)st != tx_mtu)
        {
            printf("TX Failed: %in", st);
        }        
    }

    fclose(rx_data);
    fclose(tx_data);

    return 0;
}