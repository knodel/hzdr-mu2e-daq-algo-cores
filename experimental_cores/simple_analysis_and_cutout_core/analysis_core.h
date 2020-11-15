#include "hls_stream.h"
#include "ap_int.h"
#include "ap_shift_reg.h"
#include "math.h"

//all lengths are in pairs of numbers i.e. delta_wakback of 8 = 16 samples
const long data_len = 251 * 6;
const int delta_integration = 64;
const int delta_walkback = 8;
const int delta_gap = 8;
const int pre = 32;
const int post = 128;
const int max_walkback = 8;
const int max_pre = 32;
const int analysis_len = delta_integration + delta_gap;
const int cutout_len = pre + post;
const int pulse_len = cutout_len > analysis_len ? cutout_len : analysis_len;

const int trigger_slope1 = 80;
const int trigger_slope2 = 100;
const float conversion = 0.0025;
const int log_window_mean = 6;
const int bound2 = 25;
const int bound1 = 50;

//typedef ap_uint<16> data_t;
typedef int data_t;
//typedef ap_uint<17> sum_t;
typedef int sum_t;
//typedef ap_int<32> acc_t;
typedef int acc_t;
//typedef ap_int<18> mean_t;
typedef int mean_t;

struct input_t{
	data_t value1;
	data_t value2;
};

struct analysis_t{
	long pulse_time;
	float pulse_energy;
	int quality;
};

struct cutout_t{
	long time1;
	long time2;
	data_t value1;
	data_t value2;
};


typedef hls::stream<input_t> in_stream_t;
typedef hls::stream<cutout_t> out_stream_t;
typedef hls::stream<analysis_t> analysis_stream_t;


void analysis_core(in_stream_t& in, out_stream_t& out, analysis_stream_t& out_analysis);
