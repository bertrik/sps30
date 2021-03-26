
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

int SPS30::sps_exchange(uint8_t cmd, size_t out_len, uint8_t *out_buf, uint8_t *in_buf)
{
    uint8_t buf[256];
    size_t in_len;

    // build command and send it
    shdlc.reset(cmd);
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

bool SPS30::sps_device_info(char *product_type, char *serial_number)
{
    uint8_t mosi[1];
    uint8_t miso[256];
    int len;

    // product type
    mosi[0] = 0;
    len = sps_exchange(0xd0, sizeof(mosi), mosi, miso);
    if (len <= 0) {
        return false;
    }
    strlcpy(product_type, (const char *)miso, len);
    
    // serial number
    mosi[0] = 3;
    len = sps_exchange(0xd0, sizeof(mosi), mosi, miso);
    if (len <= 0) {
        return false;
    }
    strlcpy(serial_number, (const char *)miso, len);
    
    return true;
}


bool SPS30::sps_start(bool use_float)
{
    uint8_t data[2];
    uint8_t buf[256];
    
    data[0] = 1;    // sub-command
    data[1] = use_float ? 5 : 3;
    return sps_exchange(0x00, sizeof(data), data, buf) == 0;
}


bool SPS30::sps_stop(void)
{
    uint8_t buf[256];
    
    return sps_exchange(0x01, 0, NULL, buf) == 0;
}

bool SPS30::sps_read_measurement(void)
{
    uint8_t buf[256];

    return sps_exchange(0x03, 0, NULL, buf) > 0;
}


