
#include "zero_suppression_cutout_core.h"

void findRisingEdge(in_stream_t &input, out_stream_t &output) {
    #pragma HLS INTERFACE axis port=input
    #pragma HLS INTERFACE axis port=output

    int64_t mean = 0;
    int64_t variance = 0;
    int64_t last_trigger = -PRE - POST - 1;
    int64_t old_mean;
    int64_t height;

    int64_t i;
    int16_t j;

    //have to be signed numbers so that the arithmetic below works correctly
    uint16_t waveform[WINDOW_LEN + 2];
    #pragma HLS array_partition variable=waveform cyclic factor=unroll_factor

    uint16_t new_value;
    uint16_t new_values[3];
    uint16_t old_value;
    uint16_t out_value;

    out_stream_data result;

    static ap_shift_reg<uint16_t, PRE> pre_register;
    static ap_shift_reg<uint16_t, WINDOW_LEN> window_register;

    //cache input and setup variables for find_edge loop
    cache:
    for (i = 0; i < WINDOW_LEN + 2; i++) {
        #pragma HLS unroll
        for (j = 0; j < 2; j++) {
            #pragma HLS unroll
            new_values[j] = new_values[j + 1];
        }
        input >> new_value;
        new_values[2] = new_value;
        waveform[i] = new_value;
        pre_register.shift(new_values[0]);
        window_register.shift(new_values[0]);
    }

    //calculate first mean
    mean:
    for (i = 0; i < WINDOW_LEN; i++) {
        #if unroll_factor != WINDOW_LEN
            #pragma HLS unroll factor=unroll_factor
            #pragma HLS pipeline
        #else
            #pragma HLS unroll
        #endif
        mean += waveform[i];
    }
    mean /= WINDOW_LEN;

    //calculate first variance
    variance:
    for (i = 0; i < WINDOW_LEN; i++) {
        #if unroll_factor != WINDOW_LEN
            #pragma HLS unroll factor=unroll_factor
            #pragma HLS pipeline
        #else
            #pragma HLS unroll
        #endif
        variance += (waveform[i] - mean) * (waveform[i] - mean);
    }
    variance /= WINDOW_LEN;

    find_edge:
    for (i = WINDOW_LEN + 2; i < WF_LENGTH + PRE + 2; i++) {
        #pragma HLS pipeline
        for (j = 0; j < 2; j++) {
            new_values[j] = new_values[j + 1];
        }

        if (i < WF_LENGTH) {
            input >> new_values[2];
        }
        out_value = pre_register.shift(new_values[0]);
        old_value = window_register.shift(new_values[0]);

        old_mean = mean;
        height = new_values[0] - mean;

        //trigger on rising edge
        if ((height * height > variance * THRESHOLD * THRESHOLD) &
            (height >= 0) &
            (new_values[2] > new_values[1]) &
            (new_values[1] > new_values[0]) &
            (last_trigger < i - PRE - POST)) {
            last_trigger = i;
        }

        //store values
        if ((i >= PRE + 2) & (last_trigger > (i - PRE - POST))) {
            result.time = i - PRE - 2;
            result.data = out_value;
            output << result;
        }

        mean += (new_values[0] - old_value) / WINDOW_LEN;
        variance += (new_values[0] - old_value) *
                    (new_values[0] + old_value - mean - old_mean) / WINDOW_LEN;
    }
}
