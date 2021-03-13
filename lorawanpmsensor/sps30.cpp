
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


