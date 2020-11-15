#include <hls_stream.h>
#include <ap_shift_reg.h>
#include <ap_fixed.h>

const int max_pulse_count = 10;

typedef ap_fixed<32, 8> data_t;

struct in_stream_data_t{
	data_t first_value;
	data_t second_value;
	bool end_of_pulse;
};

struct pair{
	data_t value;
	data_t half;
};

struct report{
	data_t height;
	long time;
};

typedef hls::stream<in_stream_data_t> in_stream_t;
typedef hls::stream<report> out_stream_t;

const data_t factor = 1 - 1.283446486199935e-05; //exponential decay factor
const int edge_window_m = 350; //subtract sample 2*edge_window_m samples back to get height (2*m in original code)
const int avg_window_l = 50; //average over window of size 2*g_window_l (2*l in original code)


void mwd(in_stream_t& in_stream, out_stream_t& out_stream);
