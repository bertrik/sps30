#include <Arduino.h>

#include <stdbool.h>
#include <stdint.h>

class SPS30 {

private:
    Stream *_serial;

    int sps_exchange(uint8_t cmd, size_t out_len, uint8_t *out_buf, uint8_t *in_buf);

public:
    static const int BIT_RATE = 115200;

    /**
     * Constructor.
     *
     * @param serial the serial port, NOTE: the serial port has to be configured for a bit rate of MHZ19::BIT_RATE !
     */
    explicit SPS30(Stream *serial);

    bool sps_start(bool use_float);
    bool sps_stop(void);
    bool sps_read_measurement(void);

};

