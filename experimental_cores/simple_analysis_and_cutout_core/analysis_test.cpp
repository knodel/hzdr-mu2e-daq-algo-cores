#include "analysis_core.h"
#include "stdio.h"

int main(){
	in_stream_t in;
	out_stream_t out;
	analysis_stream_t out_analysis;

	input_t tmp_in;
	cutout_t tmp_cutout;
	analysis_t tmp_analysis;

	//add a path to the data source here
	FILE* file = fopen("", "r");
	if(file == NULL){
		perror("\n\nUnable to open input file!");
		return 1;
	}
	printf("file opened\n");

	//read file
	for(int i=0; i<data_len; i++){
		if(fscanf(file, "%d", &tmp_in.value1) != 1) printf("file too short\n");
		if(fscanf(file, "%d", &tmp_in.value2) != 1) printf("file too short\n");
		in << tmp_in;
	}
	fclose(file);
	printf("data read\n");

	//execute core
	analysis_core(in, out, out_analysis);
	printf("core executed\n");

	//print results
	printf("results:\n");
	int i = 0;
	while(!out.empty()){
		out >> tmp_cutout;
		if(i < 6 || out.empty()){
			printf("%d\t%d\t\n", tmp_cutout.time1, tmp_cutout.value1);
			printf("%d\t%d\t\n", tmp_cutout.time2, tmp_cutout.value2);
		}

		i++;
	}
	i = 0;
	while(!out_analysis.empty()){
		out_analysis >> tmp_analysis;
		printf("%d\t%f\t%d\n", tmp_analysis.pulse_time, tmp_analysis.pulse_energy, tmp_analysis.quality);

		i++;
	}
	return 0;
}
