#include "pulse_integration_quality_analysis_core.h"

//return true iff the tree data points resemble an edge
//it is checked separately whether the pulse does not overlap with input boundaries or previous pulses
bool trigger(int value_t0, int value_t1, int value_t2,
		double one_mV, double threshold){
	#pragma HLS inline

	//detected if multiply of voltage difference between three consecutive data points is larger than 1mV^2
	int delta_1 = value_t0 - value_t1;
	int delta_2 = value_t1 - value_t2;

	if(delta_1 * delta_2 > one_mV * threshold * one_mV * threshold){
		return true;
	}
	else{
		return false;
	}
}

//compute output data from integrations over Front Pedestal, Main Signal, Back Pedestal
//time in number of data points taken
out_stream_data_t post_processing(int sum_PF, int sum_Integration, int sum_PB, long time,
		int delta_PF, int delta_Integration, int delta_PB, double one_mV, double bound2, double bound1){
	#pragma HLS inline
	out_stream_data_t result;
	double factor_PF = delta_Integration * (1.0 / delta_PF); //(1.0 / delta_PF) gets only calculated once (on program startup)
	double factor_PB = delta_Integration * (1.0 / delta_PB); //same deal for this one and (1 / one_mV)

	double energy = sum_Integration - factor_PF * sum_PF;

	result.pulse_energy = energy * (1 / one_mV);
	result.pulse_time = time;

	double delta_pedestal = factor_PF * sum_PF - factor_PB * sum_PB;

	if(delta_pedestal < 0){
		result.quality = 2;
	}
	else if(delta_pedestal * bound2 > fabs(energy)){
		result.quality = 2; //large difference; May have pile-up
	}
	else if(delta_pedestal * bound1 > fabs(energy)){
		result.quality = 1; //pile-up but acceptable difference
	}
	else{
		result.quality = 0; //default relative good quality
	}

	return result;
}


void analysis_core(in_stream_t& in, out_stream_t& out,
		int delta_Walkback_, int delta_PF_, int delta_Integration_, int delta_Gap_, int delta_PB_,
		double threshold_, double one_mV_, double bound2_, double bound1_){
	#pragma HLS INTERFACE axis port=in
	#pragma HLS INTERFACE axis port=out

	//cache parameters
	const int delta_Walkback = delta_Walkback_;
	const int delta_PF = delta_PF_;
	const int delta_Integration = delta_Integration_;
	const int delta_Gap = delta_Gap_;
	const int delta_PB = delta_PB_;
	const double threshold = threshold_;
	const double one_mV = one_mV_;
	const double bound2 = bound2_;
	const double bound1 = bound1_;

	//precompute derived parameters
	const int start_PF = 0;
	const int start_Integration = start_PF + delta_PF;
	const int start_Gap = start_Integration + delta_Integration;
	const int start_PB = start_Gap + delta_Gap;
	const int end_pulse = start_PB + delta_PB; //value after pulse
	const int trigger_delay = delta_PF + delta_Walkback + trigger_lookback - 1;
	const int sreg_length = (max_trigger_delay + trigger_lookback - 1) / 2;

	//values are stored in sreg in the time between pulse start and trigger
	static ap_shift_reg<in_stream_data_t, sreg_length> sreg; //static required by library

	//input values flow through these variables in order of declaration
	in_stream_data_t new_val;          //receive two values from input stream
	data_t new_vals[trigger_lookback]; //keep last three new values accessible for edge trigger
	in_stream_data_t cur_val;          //read structure with two values from shift register
	data_t cur_vals[2];                //unpack structure into cur_vals
	data_t odd_delay_compensation;     //to allow odd delay, swap in this stored value

	out_stream_data_t result;          //temporary storage for next output
	long last_trigger = -end_pulse-1;  //initialize to more than one pulse before start


	accumulate_t window_sum[2];        //compute values at odd and even times separately; save final sum to variables below
	accumulate_t sum_PF;
	accumulate_t sum_Integration;
	accumulate_t sum_PB;

	bool do_output;                    //if do_output is true the content of result is put into the out stream
	accumulate_t sum_PF_output;        //remember intermediate results for post-processing
	accumulate_t sum_Integration_output;
	accumulate_t sum_PB_output;
	long time_output;

	//initialize new_vals, window_sum gets initialized on the fly
	for(int i = 0; i < 3; i++){
		#pragma HLS unroll
		new_vals[i] = 0;
	}

	//do this loop every cycle
	for(long i = 0; i < data_len; i+=2){
		#pragma HLS pipeline

		//shift in new values from FIFO
		in >> new_val;
		cur_val = sreg.shift(new_val, trigger_delay / 2 - 1);

		if(trigger_delay%2 == 0){
			cur_vals[0] = cur_val.first_data;
			cur_vals[1] = cur_val.second_data;
		}
		else{
			cur_vals[0] = odd_delay_compensation; //first uninitialized cur_vals[0] is never used
			cur_vals[1] = cur_val.first_data;
		}
		odd_delay_compensation = cur_val.second_data;

		//determine if the time does not overlap with a previous pulse
		//if the first value triggers, edge_is_new[1] is set to false
		bool edge_is_new[2];
		long cached_last_trigger = last_trigger;
		edge_is_new[0] = cached_last_trigger < i - end_pulse;
		edge_is_new[1] = cached_last_trigger < i+1 - end_pulse;

		//work on two values every cycle
		for(int j = 0; j<2; j++){
			long iteration = i+j;

			//shift new values
			for(int k = 0; k<trigger_lookback-1; k++){
				new_vals[k] = new_vals[k+1];
			}
			if(j==0) new_vals[2] = new_val.first_data;
			else new_vals[2] = new_val.second_data;

			//trigger condition
			bool edge = trigger(new_vals[0], new_vals[1], new_vals[2], one_mV, threshold);
			bool not_cut_by_start = iteration > trigger_delay;
			if(edge & not_cut_by_start & edge_is_new[j]){
				last_trigger = iteration;
				edge_is_new[1] = false;
				//printf("triggered at %d\n", iteration);
			}

			//save window_sum to sum_PF, sum_Integration, sum_PB at appropriate time
			bool reset_sum = false;
			accumulate_t cached_window_sum = window_sum[0] + window_sum[1];
			long t = iteration - last_trigger;
			if(t == start_PF) reset_sum = true;
			if(t == start_Integration){ sum_PF = cached_window_sum; reset_sum = true; }
			if(t == start_Gap) sum_Integration = cached_window_sum;
			if(t == start_PB) reset_sum = true;
			if(t == end_pulse) sum_PB = cached_window_sum;

			if(reset_sum){
				window_sum[0] = 0;
				window_sum[1] = 0;
			}

			//post_processing and output
			if(j == 0) do_output = false;
			if(t == end_pulse){

				sum_PF_output = sum_PF;
				sum_Integration_output = sum_Integration;
				sum_PB_output = sum_PB;
				time_output = last_trigger;

				do_output = true;
			}
			if((j==1) & do_output){
				result = post_processing(sum_PF_output, sum_Integration_output, sum_PB_output,
										time_output - (trigger_lookback - 1),
										delta_PF, delta_Integration, delta_PB, one_mV, bound2, bound1);
				out << result;
			}

			//integrate
			window_sum[j] += cur_vals[j];
		}

	}
}

