#include "pulse_integration_quality_analysis_core.h"

void analysis_core(in_stream_t& in, out_stream_t& out, int trigger_delay){
	#pragma HLS INTERFACE axis port=in
	#pragma HLS INTERFACE axis port=out

	//values are stored in new_vals in the time between pulse start and trigger
	data_t new_vals[max_trigger_delay];
	#pragma HLS array_partition variable=new_vals

	data_t cur_val; //value to be added to window_sum in the current loop iteration
	report result;
	long last_trigger = -end_pulse-1; //initialize to more than one pulse before start

	data_t window_sum = 0; //compute section sums in this variable; save final sum to variables below
	double sum_PF;
	double sum_Integration;
	double sum_PB;

	//initialize
	for(int i = 0; i < max_trigger_delay; i++){
		#pragma HLS unroll
		new_vals[i] = 0;
	}

	for(long i = 0; i < data_len; i++){
		#pragma HLS pipeline

		//shift in new value from FIFO
		cur_val = new_vals[trigger_delay-1];
		for(int j = max_trigger_delay-1; j>0; j--){
			new_vals[j] = new_vals[j-1];
		}
		in >> new_vals[0];

		//trigger condition
		//detected if multiply of voltage difference between three consecutive data points is larger than 1mV^2
		double delta_1 = (new_vals[2] - new_vals[1]);
		double delta_2 = (new_vals[1] - new_vals[0]);
		bool edge = conversion2mV_trigger * delta_1 * delta_2  > 1;
		bool edge_is_new = last_trigger < i - end_pulse;
		bool not_cut_by_start = i > trigger_delay;
		if(edge && edge_is_new && not_cut_by_start) last_trigger = i;

		//save window_sum to sum_PF, sum_Integration, sum_PB at appropriate time
		bool reset_sum = false;

		int t = i - last_trigger;
		if(t == start_PF) reset_sum = true;
		if(t == start_Integration){sum_PF = window_sum; reset_sum = true;}
		if(t == start_Gap) sum_Integration = window_sum;
		if(t == start_PB) reset_sum = true;
		if(t == end_pulse) sum_PB = window_sum;

		if(reset_sum) window_sum = 0;

		if(t == end_pulse){
			double local_sum_Integration = sum_Integration;
			local_sum_Integration -= 4 * sum_PF;
			result.pulse_energy = local_sum_Integration * conversion2mV;
			result.pulse_time = last_trigger - (trigger_lookback - 1);

			result.quality = 0; //default relative good quality
			double delta_pedestal = sum_PF - sum_PB;

			if(delta_pedestal < 0 || delta_pedestal * 4 * bound2 > fabs(local_sum_Integration))
				result.quality = 2; //large difference; May have pile-up
			else if(delta_pedestal * 4 * bound1 > fabs(local_sum_Integration))
				result.quality = 1; //pile-up but acceptable difference

			out << result;
		}


		//integrate
		window_sum += cur_val;
	}
}

