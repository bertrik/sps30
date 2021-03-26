
#include <stdbool.h>
#include <stdio.h>

#include "shdlc.h"
#include "sps30.h"

static SHDLC shdlc;

static void printhex(const uint8_t * buf, int len)
{
    for (int i = 0; i < len; i++) {
        printf("%02X", buf[i]);
    }
    printf("\n");
}

SPS30::SPS30(Stream *serial)
{
    _serial = serial;
}

int SPS30::exchange(uint8_t cmd, size_t out_len, uint8_t *out_buf, uint8_t *in_buf)
{
    uint8_t buf[256];
    size_t in_len;

    // build command and send it
    int len = shdlc.build_mosi(buf, cmd, out_len, out_buf);
    _serial->write(buf, len);
    printhex(buf, len);

    // wait for result
    unsigned long start = millis();
    while ((millis() - start) < 100) {
        while (_serial->available()) {
            char c = _serial->read();
            if (shdlc.proc_miso(c, cmd)) {
                in_len = shdlc.get_data(in_buf);
                printhex(in_buf, in_len);
                return in_len;
            }
        }
    }
    return -1;
}  

bool SPS30::device_info(char *product_type, char *serial_number)
{
    uint8_t mosi[1];
    uint8_t miso[256];
    int len;

    // product type
    mosi[0] = 0;
    len = exchange(0xd0, sizeof(mosi), mosi, miso);
    if (len <= 0) {
        return false;
    }
    strlcpy(product_type, (const char *)miso, len);
    
    // serial number
    mosi[0] = 3;
    len = exchange(0xd0, sizeof(mosi), mosi, miso);
    if (len <= 0) {
        return false;
    }
    strlcpy(serial_number, (const char *)miso, len);
    
    return true;
}


bool SPS30::start(bool use_float)
{
    uint8_t data[2];
    uint8_t buf[256];
    
    data[0] = 1;    // sub-command
    data[1] = use_float ? 3 : 5;
    return exchange(0x00, sizeof(data), data, buf) == 0;
}


bool SPS30::stop(void)
{
    uint8_t buf[256];
    
    return exchange(0x01, 0, NULL, buf) == 0;
}

bool SPS30::read_measurement(uint16_t *pm1_0, uint16_t *pm2_5, uint16_t *pm4_0, uint16_t *pm10, uint16_t *ps)
{
    uint8_t miso[40];

    int len = exchange(0x03, 0, NULL, miso);
    if (len < 20) {
        return false;
    }
    *pm1_0 = (miso[0] << 8) + miso[1];
    *pm2_5 = (miso[2] << 8) + miso[3];
    *pm4_0 = (miso[4] << 8) + miso[5];
    *pm10 = (miso[6] << 8) + miso[7];
    *ps = (miso[18] << 8) + miso[19];
    return true;
}


