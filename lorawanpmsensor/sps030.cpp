
#include <stdbool.h>
#include <stdio.h>


#include "sps030.h"



// true if *pc contains a valid value
bool SPS030::unescape(uint8_t *pc)
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



void SPS030::reset(uint8_t cmd)
{
    _cmd = cmd;
    _state = SPS030_START;
    _escape = false;
}


bool SPS030::process(uint8_t c)
{
//    printf("state=%d, char=0x%02X\n", _state, c);

    switch (_state) {
    case SPS030_START:
        if (c == 0x7E) {
            _state = SPS030_ADR;
            _escape = false;
        }
        break;
    case SPS030_ADR:
        if (unescape(&c)) {
            _check = c;
            _state = SPS030_CMD;
        }
        break;
    case SPS030_CMD:
        if (unescape(&c)) {
            if (c == _cmd) {
                _check += c;
                _state = SPS030_STATE;
            } else {
                _state = SPS030_START;
            }
        }
        break;
    case SPS030_STATE:
        if (unescape(&c)) {
            _check += c;
            _miso_state = c;
            _state = SPS030_L;
        }
        break;
    case SPS030_L:
        if (unescape(&c)) {
            _check += c;
            _index = 0;
            _length = c;
            if (_length > 0) {
                _state = SPS030_DATA;
            } else {
                _state = SPS030_CHK;
            }
        }
        break;
    case SPS030_DATA:
        if (unescape(&c)) {
            _check += c;
            _data[_index++] = c;
            if (_index == _length) {
                _state = SPS030_CHK;
            }    
        }
        break;
    case SPS030_CHK:
        if (unescape(&c)) {
            _check ^= 0xFF;
            if (_check == c) {
                _state = SPS030_STOP;
            } else {
                printf("check=0x%02X\n", _check);
                _state = SPS030_START;
            }
        }
        break;
    case SPS030_STOP:
        if (c == 0x7E) {
            _state = SPS030_START;
            return true;
        } else {
            _state = SPS030_START;
        }
        break;
    default:
        _state = SPS030_START;
        break;
    }

    return false;    
}


