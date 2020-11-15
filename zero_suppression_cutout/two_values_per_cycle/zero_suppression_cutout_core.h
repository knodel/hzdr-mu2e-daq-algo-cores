
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "hls_stream.h"
#include "ap_shift_reg.h"

//**CHANGEABLE PARAMETERS**
#define LINE_LENGTH 10000
#define WF_LENGTH 1019
#define PRE 256
#define POST 256
#define WINDOW_LEN 128
#define THRESHOLD 4.0

//calculate unroll_factor for HLS pragmas
template<int16_t number, bool state>
struct divisibility {
    enum {
        value = divisibility<number + 1,
                (WINDOW_LEN % (number + 1) == 0)>::value
    };
};

template<int16_t number>
struct divisibility<number, true> {
    enum {
        value = number
    };
};

const int16_t unroll_factor = divisibility<1, false>::value;

//**INPUT AND OUTPUT TYPES**
struct in_stream_data {
    uint16_t first_data;
    uint16_t second_data;
};

struct out_stream_data {
    uint16_t first_data;
    uint16_t second_data;
    int64_t first_time;
    int64_t second_time;
};

typedef hls::stream <in_stream_data> in_stream_t;
typedef hls::stream <out_stream_data> out_stream_t;

//**FUNCTION PROTOTYPES**
void findRisingEdge(in_stream_t &input, out_stream_t &output);

void importWaveform(uint16_t *waveform, std::string path);

void printWaveform(uint16_t *waveform, uint16_t length);

void executeCore(int64_t *time, uint16_t *data, uint16_t *waveform, uint16_t &length);

void printWindow(int64_t *time, uint16_t *data, uint16_t length);

void writeWindow(int64_t *time, uint16_t *data, uint16_t length, std::string path);

