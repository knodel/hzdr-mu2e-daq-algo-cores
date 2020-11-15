#include "analysis_core.h"

void analysis_core(in_stream_t& in, out_stream_t& out, analysis_stream_t& out_analysis){
	input_t tmp_in;
	cutout_t tmp_cutout;
	analysis_t tmp_analysis;

	static ap_shift_reg<sum_t, max_walkback> walkback_sreg;
	static ap_shift_reg<input_t, max_pre> cutout_sreg;

	mean_t mean;
	acc_t acc;
	acc_t final_sum;

	//fill walkback register
	for(long i = 0; i < delta_walkback; i++){
		#pragma HLS pipeline
		in >> tmp_in;
		walkback_sreg.shift(tmp_in.value1 + tmp_in.value2);
		cutout_sreg.shift(tmp_in);
		mean = tmp_in.value1 + tmp_in.value2;
	}

	long last_trigger = -pulse_len - 1;
	long t = pulse_len + 1;

	for(long i = delta_walkback; i < data_len + pre; i++){
		#pragma HLS pipeline
		data_t last_value = tmp_in.value2;
		if(i < data_len) in >> tmp_in;

		bool new_edge = t >= analysis_len - 1;
		t = i - last_trigger;
		if(last_value - trigger_slope1 > tmp_in.value1 &&
				tmp_in.value1 - trigger_slope2 > tmp_in.value2 &&
				new_edge){
			last_trigger = i;
			acc = 0;
			t = 0;
		}

		sum_t new_val = tmp_in.value1 + tmp_in.value2;
		sum_t cur_val = walkback_sreg.shift(new_val, delta_walkback-1);
		input_t out_vals = cutout_sreg.shift(tmp_in, pre-1);

		acc += cur_val - acc_t(mean);

		if(t == delta_integration - 1){
			final_sum = acc;
		}

		if(t == delta_integration + delta_gap - 1){
			tmp_analysis.pulse_energy = final_sum * conversion;
			tmp_analysis.pulse_time = last_trigger * 2;

			acc_t delta = (mean - new_val) * delta_integration;
			if(delta < 0)                             tmp_analysis.quality = 2;
			else if(delta * bound2 > fabs(final_sum)) tmp_analysis.quality = 2;
			else if(delta * bound1 > fabs(final_sum)) tmp_analysis.quality = 1;
			else                                      tmp_analysis.quality = 0;

			out_analysis << tmp_analysis;
			//printf("mean: %d\n", mean);
		}

		if(t >= delta_integration + delta_gap){
			mean += (cur_val - mean) >> log_window_mean;
		}

		if(t < cutout_len && i >= pre){
			tmp_cutout.time1 = (i - pre) * 2;
			tmp_cutout.time2 = (i - pre) * 2 + 1;
			tmp_cutout.value1 = out_vals.value1;
			tmp_cutout.value2 = out_vals.value2;
			out << tmp_cutout;
		}

	}
}
