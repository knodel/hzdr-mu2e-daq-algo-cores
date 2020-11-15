#include <hls_stream.h>
#include <ap_shift_reg.h>
#include <ap_fixed.h>

const int max_pulse_count = 10;

typedef ap_fixed<32, 8> data_t;

struct in_stream_data_t{
	data_t value;
	bool end_of_pulse;
};

struct report{
	data_t height;
	long time;
};

typedef hls::stream<in_stream_data_t> in_stream_t;
typedef hls::stream<report> out_stream_t;

const data_t factor = 1 - 1.283446486199935e-05; //exponential decay factor
const int edge_window_m = 700; //subtract sample edge_window_m back to get height (m in original code)
const int avg_window_l = 100; //average over window of size avg_window_l (l in original code)


void mwd(in_stream_t& in_stream, out_stream_t& out_stream);
