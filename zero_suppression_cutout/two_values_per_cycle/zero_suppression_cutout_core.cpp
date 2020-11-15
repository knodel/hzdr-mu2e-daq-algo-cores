
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
    int16_t k;

    bool execute_output;
    bool new_edge[2];
    bool output_lock = true;

    uint16_t waveform[WINDOW_LEN + 3];
    #pragma HLS array_partition variable=waveform cyclic factor=unroll_factor

    //have to be signed numbers so that the arithmetic below works correctly
    uint16_t new_values[3];
    uint16_t old_value;
    uint16_t output_value;
    uint16_t current_old_values[2];
    uint16_t current_output_values[2];
    uint16_t old_value_delay_compensation;
    uint16_t output_value_delay_compensation;


    in_stream_data new_struct;
    in_stream_data old_values;
    in_stream_data output_values;

    out_stream_data result;
    out_stream_data cached_result;

    static ap_shift_reg<in_stream_data, PRE / 2 + 1> pre_register;
    static ap_shift_reg<in_stream_data, WINDOW_LEN / 2 + 1> window_register;

    //cache input and setup variables for find_edge loop
    cache:
    for (i = 0; i < WINDOW_LEN + 2; i += 2) {
        #pragma HLS unroll
        input >> new_struct;
        new_values[0] = new_values[2];
        new_values[1] = new_struct.first_data;
        new_values[2] = new_struct.second_data;
        waveform[i] = new_struct.first_data;
        waveform[i + 1] = new_struct.second_data;
        output_values = pre_register.shift(new_struct);
        old_values = window_register.shift(new_struct);
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
    for (i = WINDOW_LEN - (WINDOW_LEN % 2); i < WF_LENGTH + PRE + (WF_LENGTH % 2); i += 2) {
        #pragma HLS pipeline

        //ignore first input if WINDOW_LEN is odd
        if (i >= WINDOW_LEN) {
            if (i < WF_LENGTH - 2) {
                input >> new_struct;
            }
            output_values = pre_register.shift(new_struct);
            old_values = window_register.shift(new_struct);
        }

        //faster access of current_output_values and current_old_values in the loop below
        if (PRE % 2 == 0) {
            current_output_values[0] = output_values.first_data;
            current_output_values[1] = output_values.second_data;
        } else {
            current_output_values[0] = output_value_delay_compensation;
            current_output_values[1] = output_values.first_data;
        }
        output_value_delay_compensation = output_values.second_data;

        if (WINDOW_LEN % 2 == 0) {
            current_old_values[0] = old_values.first_data;
            current_old_values[1] = old_values.second_data;
        } else {
            current_old_values[0] = old_value_delay_compensation;
            current_old_values[1] = old_values.first_data;
        }
        old_value_delay_compensation = old_values.second_data;

        //faster access at trigger condition in the loop below
        new_edge[0] = last_trigger < (i - PRE - POST);
        new_edge[1] = last_trigger < (i + 1 - PRE - POST);

        execute_output = false;

        //process two values in one cycle
        for (j = 0; j < 2; j++) {
            //ignore first value if WINDOW_LEN is odd
            if (i >= WINDOW_LEN) {
                for (k = 0; k < 2; k++) {
                    new_values[k] = new_values[k + 1];
                }

                if (i < WF_LENGTH - 2) {
                    if ((j % 2) == 0) {
                        new_values[2] = new_struct.first_data;
                    } else {
                        new_values[2] = new_struct.second_data;
                    }
                }
            }

            output_value = current_output_values[j];
            old_value = current_old_values[j];

            old_mean = mean;
            height = new_values[0] - mean;

            // trigger on rising edge
            if ((height * height > variance * THRESHOLD * THRESHOLD) &
                (height >= 0) &
                (new_values[2] > new_values[1]) &
                (new_values[1] > new_values[0]) &
                new_edge[j] & (i + j >= WINDOW_LEN)) {
                last_trigger = i + j;
                new_edge[1] = false;
            }

            //store every two iterations two values
            if (((i + j) >= PRE) & (last_trigger >= (i + j - PRE - POST))) {
                if (output_lock) {
                    result.first_time = i + j - PRE;
                    result.first_data = output_value;
                } else {
                    result.second_time = i + j - PRE;

                    if (((i + j - PRE - POST) == last_trigger) &
                        ((PRE % 2) ^ (POST % 2))) {
                        result.second_data = 0;
                    } else {
                        result.second_data = output_value;
                    }
                    cached_result = result;
                    execute_output = true;
                }
                output_lock = !output_lock;
            }

            mean += (i + j >= WINDOW_LEN) * (new_values[0] - old_value) / WINDOW_LEN;
            variance += (i + j >= WINDOW_LEN) * (new_values[0] - old_value) *
                        (new_values[0] + old_value - mean - old_mean) / WINDOW_LEN;
        }

        if (execute_output) {
            output << cached_result;
        }
    }
}
