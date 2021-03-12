// simple test code to verify creation of tx command and parsing of rx response 

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "sps030.h"

static bool validate_rx(const char *name, uint8_t cmd, size_t len, uint8_t buf[])
{
    SPS030 sps;
    
    printf("Running test '%s'...", name);
    sps.reset(cmd);
    bool b;
    for (size_t i = 0; i < len; i++) {
        b = sps.process(buf[i]);
    }
    printf("%s\n", b ? "OK" : "FAIL");
    return b;
}


static bool test_rx(void)
{
    SPS030 sps;

    sps.reset(3);
    sps.process(0x7E);
    sps.process(0x00);
    sps.process(0x03); // cmd
    sps.process(0x00); // state
    sps.process(0x00); // length
    sps.process(0xFC);
    bool b = sps.process(0x7E);
    
    printf("\n");
    sps.reset(0xD0);
    uint8_t cmd[] = {
        0x7E, 0x00, 0xD0, 0x00, 0x15, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
        0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x5A, 0x7E
    };
    for (size_t i = 0; i < sizeof(cmd); i++) {
        b = sps.process(cmd[i]);
    }
    
    uint8_t cmd_start_measurement[] = {
        0x7E, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7E
    };
    validate_rx("cmd_start_measurement", 0x00, sizeof(cmd_start_measurement), cmd_start_measurement);

    uint8_t cmd_stop_measurement[] = {
        0x7E, 0x00, 0x01, 0x00, 0x00, 0xFE, 0x7E
    };
    validate_rx("cmd_stop_measurement", 0x01, sizeof(cmd_stop_measurement), cmd_stop_measurement);

    uint8_t cmd_read_measured_values[] = {
        0x7E, 0x00, 0x03, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xD4, 0x7E
    };
    validate_rx("cmd_read_measured_values", 0x03, sizeof(cmd_read_measured_values), cmd_read_measured_values);

    uint8_t cmd_sleep[] = {
        0x7E, 0x00, 0x10, 0x00, 0x00, 0xEF, 0x7E
    };
    validate_rx("cmd_sleep", 0x10, sizeof(cmd_sleep), cmd_sleep);

    uint8_t cmd_wake_up[] = {
        0x7E, 0x00, 0x7D, 0x31, 0x00, 0x00, 0xEE, 0x7E
    };
    validate_rx("cmd_wake_up", 0x11, sizeof(cmd_wake_up), cmd_wake_up);

    uint8_t cmd_clean_fan[] = {
        0x7E, 0x00, 0x56, 0x00, 0x00, 0xA9, 0x7E
    };
    validate_rx("cmd_clean_fan", 0x56, sizeof(cmd_clean_fan), cmd_clean_fan);
    
    uint8_t cmd_read_auto_cleaning_interval[] = {
        0x7E, 0x00, 0x80, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x7B, 0x7E
    };
    validate_rx("cmd_read_auto_cleaning_interval", 0x80, sizeof(cmd_read_auto_cleaning_interval), cmd_read_auto_cleaning_interval);

    uint8_t cmd_write_auto_cleaning_interval[] = {
        0x7E, 0x00, 0x80, 0x00, 0x00, 0x7F, 0x7E
    };
    validate_rx("cmd_write_auto_cleaning_interval", 0x80, sizeof(cmd_write_auto_cleaning_interval), cmd_write_auto_cleaning_interval);

    uint8_t cmd_device_information[] = {
        0x7E, 0x00, 0xD0, 0x00, 0x09, 0x30, 0x30, 0x30, 0x38, 0x30, 0x30, 0x30, 0x30, 0x00, 0x9B, 0x7E
    };
    validate_rx("cmd_device_information", 0xD0, sizeof(cmd_device_information), cmd_device_information);

    uint8_t cmd_read_version[] = {
        0x7E, 0x00, 0xD1, 0x01, 0x07, 0x02, 0x01, 0x00, 0x06, 0x00, 0x02, 0x00, 0x1C, 0x7E
    };
    validate_rx("cmd_read_version", 0xD1, sizeof(cmd_read_version), cmd_read_version);

    return b;    
}

#if 0
static bool test_tx(void)
{
    int res;

    // verify empty buffer
    res = prepare_tx(0, NULL, NULL, 0);
    if (res > 0) {
        fprintf(stderr, "expected failure for too small buffer!");
        return false;
    }

    // verify valid message
    uint8_t buf[9];
    uint8_t data[] = {0, 0, 0, 0, 0};
    res = prepare_tx(0x86, data, buf, sizeof(buf));
    if (res != 9) {
        fprintf(stderr, "expected 9 bytes!");
        return false;
    }

    const uint8_t expected[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
    return (memcmp(expected, buf, sizeof(expected)) == 0);
}
#endif

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    bool b;

    printf("test_rx ...");
    b = test_rx();
    printf("%s\n", b ? "PASS" : "FAIL");

//    printf("test_tx ...");
//    b = test_tx();
//    printf("%s\n", b ? "PASS" : "FAIL");

    return 0;
}


