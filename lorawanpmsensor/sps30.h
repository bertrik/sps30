#include <Arduino.h>

#include <stdbool.h>
#include <stdint.h>

class SPS30 {

private:
    Stream *_serial;
    uint8_t _mosi[256];
    uint8_t _miso[256];

    int exchange(uint8_t cmd, size_t out_len);

public:
    static const int BIT_RATE = 115200;

    /**
     * Constructor.
     *
     * @param serial the serial port, NOTE: the serial port has to be configured for a bit rate of SPS30::BIT_RATE !
     */
    explicit SPS30(Stream *serial);

    bool device_info(char *product_type, char *serial_number);
    bool start(bool use_float);
    bool stop(void);
    bool read_measurement(uint16_t *pm1_0, uint16_t *pm2_5, uint16_t *pm4_0, uint16_t *pm10, uint16_t *ps);
    bool clean_fan(void);

};

