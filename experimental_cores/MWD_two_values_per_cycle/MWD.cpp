#include "MWD.h"

//#define DEBUG_PRINT

#ifdef DEBUG_PRINT
#include <iostream>
#endif

void mwd(in_stream_t& in_stream, out_stream_t& out_stream){
	in_stream_data_t input_element;
	data_t last_in = 0; //second value of previous input element

	pair cur_A; //corresponds to A[k] and A[k+1] in original code
	static ap_shift_reg<pair, edge_window_m> sreg_A;
	pair old_A; //corresponds to A[k-m] and A[k-m+1] in original code

	pair cur_MWD; //corresponds to MWD[h-1] and MWD[h] in original code
	static ap_shift_reg<pair, avg_window_l> sreg_MWD;
	pair old_MWD; //corresponds to MWD[h-l-1] and MWD[h-l] in original code

	pair T; //corresponds to two current elements of T in original code

	data_t current_max = 0;
	long current_max_time = -1;

	report result;

	long absolute_time = 0;
	for(long i = -1; !in_stream.empty(); i++){
		#pragma HLS loop_tripcount min=1250 max=1251
		#pragma HLS pipeline

		//read date from input stream
		last_in = input_element.second_value;
		in_stream >> input_element;

		//1st stage: exponential decay suppression
		if(i >= 0){
			cur_A.half = input_element.first_value - factor*last_in + cur_A.value;
			cur_A.value += input_element.second_value + (1 - factor)*input_element.first_value - factor*last_in;
			old_A = sreg_A.shift(cur_A);
		}
		else cur_A.value = 0;

		data_t my_A = 0;
		if(i >= edge_window_m){//i
			//2nd stage: difference before and after potential rising edge (height)
			cur_MWD.half = cur_A.half - old_A.half;
			cur_MWD.value = cur_A.value - old_A.value;
			old_MWD = sreg_MWD.shift(cur_MWD);

			//3rd stage: compute average height
			if(i < edge_window_m + avg_window_l){
				old_MWD.half = 0;
				old_MWD.value = 0;
			}

			T.half = (cur_MWD.half - old_MWD.half) + T.value;
			data_t my_B = (cur_MWD.half - old_MWD.half);
			data_t my_A = (cur_MWD.value - old_MWD.value + my_B);
			//T.value += my_A;
		}
		//else T.value = 0;
		T.value = (T.value + my_A) * (i >= edge_window_m); //encodes both uncommented lines above

		//maintain maximum height
		if(T.half > T.value){
			if(T.half > current_max){
				current_max_time = 2*absolute_time;
				current_max = T.half;
			}
		}
		else{
			if(T.value > current_max){
				current_max_time = 2*absolute_time+1;
				current_max = T.value;
			}
		}


		//write data to output stream
		if(input_element.end_of_pulse){
			result.height = current_max * data_t(1.0/avg_window_l);
			result.time = current_max_time;
			out_stream << result;
		}

		//reset everything (indirectly)
		if(input_element.end_of_pulse) i = -1;
		if(input_element.end_of_pulse) {
			current_max_time = -1;
			current_max = 0;
		}

		absolute_time += 1;

#ifdef DEBUG_PRINT
		std::cout << "i0=" << 2*i << "\tA=" << cur_A.half << "\tMWD=" << cur_MWD.half<< "\tT=" << T.half << std::endl;
		std::cout << "i1=" << 2*i+1 << "\tA=" << cur_A.value << "\tMWD=" << cur_MWD.value << "\tT=" << T.value << "\tmax=" << current_max << std::endl;
#endif
	}
}
