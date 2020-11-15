#include "hls_stream.h"
#include "ap_int.h"
//#include "stdint.h"
#include "ap_shift_reg.h"
#include "math.h"

const int data_len = 502;// * 10100;//*605; //current stopping mechanism 5070200 or 303710 on new data: 1325868;


//**FIXED PARAMETERS**
const int trigger_lookback = 3; //keep last three new values accessible for edge trigger
const int max_trigger_delay = 60; //delta_PF + delta_Walkback may not exceed max_trigger_delay

//**INPUT AND OUTPUT TYPES**
typedef ap_uint<16> data_t;
//typedef uint16_t data_t;

typedef ap_uint<25> accumulate_t; //overflow might occur if delta_PF, delta_Integration or delta_PB is larger than 512

struct in_stream_data_t{
	data_t first_data; //array is not assignable
	data_t second_data;
};


struct out_stream_data_t{
	long pulse_time;
	double pulse_energy;
	int quality;
};


typedef hls::stream<in_stream_data_t> in_stream_t;
typedef hls::stream<out_stream_data_t> out_stream_t;

//**FUNCTION PROTOTYPES**
bool trigger(int value_t0, int value_t1, int value_t2,
		double one_mV, double threshold);

out_stream_data_t post_processing(int sum_PF, int sum_Integration, int sum_PB, long time,
		int delta_PF, int delta_Integration, int delta_PB, double one_mV, double bound2, double bound1);

void analysis_core(in_stream_t& in, out_stream_t& out,
		int delta_Walkback, int delta_PF, int delta_Integration, int delta_Gap, int delta_PB,
		double threshold, double one_mV, double bound2, double bound1);

