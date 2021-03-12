#include <stdbool.h>
#include <stdint.h>

typedef enum {
    SPS030_START,
    SPS030_ADR,
    SPS030_CMD,
    SPS030_STATE,
    SPS030_L,
    SPS030_DATA,
    SPS030_CHK,
    SPS030_STOP
} state_t;



class SPS030 {

private:
    state_t _state;
    uint8_t _cmd;
    int _index;
    int _length;
    uint8_t _data[256];
    bool _escape;
    uint8_t _check;
    uint8_t _miso_state;

private:

    bool unescape(uint8_t *pc);

public:
    static const int BIT_RATE = 115200;

    /**
     * Constructor.
     *
     * @param serial the serial port, NOTE: the serial port has to be configured for a bit rate of MHZ19::BIT_RATE !
     */
//    explicit MHZ19(Stream *serial);

    bool process(uint8_t c);
    
    void reset(uint8_t cmd);

};

