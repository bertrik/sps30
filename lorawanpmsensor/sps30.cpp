
#include <stdbool.h>
#include <stdio.h>

#include "shdlc.h"
#include "sps30.h"

#define DEFAULT_TIMEOUT 100

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

int SPS30::exchange(uint8_t cmd, size_t out_len)
{
    uint8_t buf[256];
    size_t in_len;

    // build command and send it
    int len = shdlc.build_mosi(buf, cmd, out_len, _mosi);
    _serial->write(buf, len);
    printhex(buf, len);

    // wait for result
    unsigned long start = millis();
    while ((millis() - start) < DEFAULT_TIMEOUT) {
        while (_serial->available()) {
            char c = _serial->read();
            if (shdlc.proc_miso(c, cmd)) {
                in_len = shdlc.get_data(_miso);
                printhex(_miso, in_len);
                return in_len;
            }
        }
    }
    return -1;
}  

bool SPS30::device_info(char *product_type, char *serial_number)
{
    int len;

    // product type
    _mosi[0] = 0;
    len = exchange(0xd0, 1);
    if (len <= 0) {
        return false;
    }
    strlcpy(product_type, (const char *)_miso, len);
    
    // serial number
    _mosi[0] = 3;
    len = exchange(0xd0, 1);
    if (len <= 0) {
        return false;
    }
    strlcpy(serial_number, (const char *)_miso, len);
    
    return true;
}


bool SPS30::start(bool use_float)
{
    _mosi[0] = 1;    // sub-command
    _mosi[1] = use_float ? 3 : 5;
    return exchange(0x00, 2) == 0;
}


bool SPS30::stop(void)
{
    return exchange(0x01, 0) == 0;
}

bool SPS30::read_measurement(uint16_t *pm1_0, uint16_t *pm2_5, uint16_t *pm4_0, uint16_t *pm10, uint16_t *ps)
{
    int len = exchange(0x03, 0);
    if (len < 20) {
        return false;
    }
    *pm1_0 = (_miso[0] << 8) + _miso[1];
    *pm2_5 = (_miso[2] << 8) + _miso[3];
    *pm4_0 = (_miso[4] << 8) + _miso[5];
    *pm10 = (_miso[6] << 8) + _miso[7];
    *ps = (_miso[18] << 8) + _miso[19];
    return true;
}


