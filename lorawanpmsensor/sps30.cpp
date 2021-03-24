
#include <stdbool.h>
#include <stdio.h>


#include "sps30.h"



// true if *pc contains a valid value
bool SPS30::unescape(uint8_t *pc)
{
    if (_escape) {
        _escape = false;
        *pc ^= 0x20;
        return true;
    }
    if (*pc == 0x7D) {
        _escape = true;
        return false;
    }
    return true;
}

static int add_byte(uint8_t *buf, int &idx, uint8_t b)
{
    switch (b) {
    case 0x7E:
    case 0x7D:
    case 0x11:
    case 0x13:
        buf[idx++] = 0x7D;
        buf[idx++] = b ^ 0x20;
        break;
    default:
        buf[idx++] = b;
        break;
    }
    return b;
}

int SPS30::build(uint8_t *buf, uint8_t cmd, size_t data_len, const uint8_t *data)
{
    uint8_t sum = 0;
    int index = 0;
    buf[index++] = 0x7E;
    sum += add_byte(buf, index, 0);
    sum += add_byte(buf, index, cmd);
    sum += add_byte(buf, index, data_len);
    for (int i = 0; i < data_len; i++) {
        sum += add_byte(buf, index, data[i]);
    }
    buf[index++] = sum ^ 0xFF;
    buf[index++] = 0x7E;
    return index;
}

void SPS30::reset(uint8_t cmd)
{
    _cmd = cmd;
    _state = SPS30_START;
    _escape = false;
}


bool SPS30::process(uint8_t c)
{
//    printf("state=%d, char=0x%02X\n", _state, c);

    switch (_state) {
    case SPS30_START:
        if (c == 0x7E) {
            _state = SPS30_ADR;
            _escape = false;
        }
        break;
    case SPS30_ADR:
        if (unescape(&c)) {
            _check = c;
            _state = SPS30_CMD;
        }
        break;
    case SPS30_CMD:
        if (unescape(&c)) {
//            if (c == _cmd) {
            if (true) {
                _check += c;
                _state = SPS30_STATE;
            } else {
                _state = SPS30_START;
            }
        }
        break;
    case SPS30_STATE:
        if (unescape(&c)) {
            _check += c;
            _miso_state = c;
            _state = SPS30_L;
        }
        break;
    case SPS30_L:
        if (unescape(&c)) {
            _check += c;
            _index = 0;
            _length = c;
            if (_length > 0) {
                _state = SPS30_DATA;
            } else {
                _state = SPS30_CHK;
            }
        }
        break;
    case SPS30_DATA:
        if (unescape(&c)) {
            _check += c;
            _data[_index++] = c;
            if (_index == _length) {
                _state = SPS30_CHK;
            }    
        }
        break;
    case SPS30_CHK:
        if (unescape(&c)) {
            _check ^= 0xFF;
            if (_check == c) {
                _state = SPS30_STOP;
            } else {
                printf("check=0x%02X\n", _check);
                _state = SPS30_START;
            }
        }
        break;
    case SPS30_STOP:
        if (c == 0x7E) {
            _state = SPS30_START;
            return true;
        } else {
            _state = SPS30_START;
        }
        break;
    default:
        _state = SPS30_START;
        break;
    }

    return false;    
}


