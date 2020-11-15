#include "hls_stream.h"
#include "ap_int.h"
#include "ap_shift_reg.h"
#include "math.h"

//**PARAMETERS TO BE DETERMIND** (can theoretically change during runtime)
//delta_* parameters expressed in number of data points taken
#define delta_PF 32           //length of time of front pedestal (baseline)
#define delta_Integration 128 //region of signal integration; algorithm depends on delta_Integraion = 4*delta_PF
#define delta_Gap 16          //unused time gap to allow complete pulse decay
#define delta_PB 32           //pulse back pedestal estimator; algorithm depends on delta_PF == delta_PB

#define delta_Walkback 16     //time required to account for pulse rising edge before trigger
#define data_len 502          //*605//9700 //or implement other stopping mechanism

#define conversion2mV 1000    //conversion factor of input values to millie-volts
#define conversion2mV_trigger (1024*1024) //conversion factor for trigger (might be 1000*1000)
#define bound2 25             //threshold for quality=2
#define bound1 50             //threshold for quality=1
//bounds depend on delta_PF, delta_Integration, delta_PB
//see implementation for details

//**DERIVED PARAMETERS**
#define start_PF (0)
#define start_Integration (start_PF + delta_PF)
#define start_Gap (start_Integration + delta_Integration)
#define start_PB (start_Gap + delta_Gap)
#define end_pulse (start_PB + delta_PB)

//trigger delay might be calculated at runtime in the future but may not exceed max_trigger_delay
#define trigger_delay_ (delta_PF + delta_Walkback + trigger_lookback - 1)

//**STATIC PARAMETERS** (can not change during runtime)
#define max_trigger_delay 60//trigger_delay
#define trigger_lookback 3 //keep last three new values accessible for edge trigger


struct report{
	long pulse_time;
	double pulse_energy;
	int quality;
};

typedef ap_fixed<51,20> data_t;
//typedef ap_fixed<64,32> data_t;
//typedef double data_t;
//typedef int data_t;
typedef hls::stream<data_t> in_stream_t;
typedef hls::stream<report> out_stream_t;

void analysis_core(in_stream_t& in, out_stream_t& out, int trigger_delay);

