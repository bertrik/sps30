#include <stdbool.h>
#include <stdint.h>

typedef enum {
    SPS30_START,
    SPS30_ADR,
    SPS30_CMD,
    SPS30_STATE,
    SPS30_L,
    SPS30_DATA,
    SPS30_CHK,
    SPS30_STOP
} state_t;



class SPS30 {

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

