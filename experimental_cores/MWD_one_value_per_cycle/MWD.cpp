#include "MWD.h"

//#define DEBUG_PRINT

#ifdef DEBUG_PRINT
#include <iostream>
#endif

void mwd(in_stream_t& in_stream, out_stream_t& out_stream){
	in_stream_data_t input_element;
	data_t cur_in = 0; //current input element
	data_t last_in = 0;

	data_t cur_A = 0; //corresponds to A[k] in original code
	static ap_shift_reg<data_t, edge_window_m> sreg_A;
	data_t old_A; //corresponds to A[k-m] in original code

	data_t cur_MWD = 0; //corresponds to MWD[h-1] in original code
	static ap_shift_reg<data_t, avg_window_l> sreg_MWD;
	data_t old_MWD; //corresponds to MWD[h-l-1] in original code

	data_t T = 0; //corresponds to current element of T in original code

	data_t current_max = 0;
	long current_max_time = -1;

	report result;

	long absolute_time = 0;
	for(long i = 0; !in_stream.empty(); i++){
		#pragma HLS loop_tripcount min=2501 max=2501
		#pragma HLS pipeline

		//read date from input stream
		last_in = cur_in;
		in_stream >> input_element;
		cur_in = input_element.value;

		//1st stage: exponential decay suppression
		if(i >= 1){
			cur_A += cur_in - factor*last_in;
			old_A= sreg_A.shift(cur_A);
		}
		else cur_A = 0;

		if(i >= edge_window_m + 1){
			//2nd stage: difference before and after potential rising edge
			cur_MWD = cur_A - old_A;
			old_MWD = sreg_MWD.shift(cur_MWD);

			//3rd stage: compute average height
			if(i < edge_window_m + avg_window_l + 1) old_MWD = 0;
			T += cur_MWD - old_MWD;
		}
		else T = 0;

		//maintain maximum height
		if(T > current_max){
			current_max_time = absolute_time;
			current_max = T;
		}

		//write data to output stream
		if(input_element.end_of_pulse){
			result.height = current_max * data_t(1.0/avg_window_l);
			result.time = current_max_time;
			out_stream << result;
		}

		//reset everything
		if(input_element.end_of_pulse) i = -1;
		if(input_element.end_of_pulse){
			current_max_time = -1;
			current_max = 0;
		}
		absolute_time += 1;

#ifdef DEBUG_PRINT
		std::cout << "i=" << i << "\tA=" << cur_A << "\tMWD=" << cur_MWD << "\tT=" << T << "\tmax=" << current_max << std::endl;
#endif
	}
}
