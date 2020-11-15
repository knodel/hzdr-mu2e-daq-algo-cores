#include "pulse_integration_quality_analysis_core.h"
#include "stdio.h"

int main(int argc, char* argv[]){
	in_stream_t in;
	out_stream_t out;
	report result;
	double value = 0;

	FILE* file = NULL;
	if(argc >= 2) file = fopen(argv[1], "r");
	else file = fopen("", "r");


	if(file == NULL){
		perror("\n\nUnable to open input file!");
		return 1;
	}
	printf("file opened\n");

	//read file
	for(int i=0; i<data_len; i++){
		if(fscanf(file, "%lf", &value) != 1) printf("file too short\n");
		in << value;
	}
	fclose(file);
	file = NULL;
	printf("data read\n");

	//execute core
	analysis_core(in, out, trigger_delay_);
	printf("core executed\n");

	//print results
	if(argc == 3){
		file = fopen(argv[2], "w");
		if(!file) perror("output file not opened!");
	}

	printf("results:\n");
	while(!out.empty()){
		out >> result;
		//printf("   energy: %f, time: %d, quality: %d\n", result.pulse_energy, result.pulse_time, result.quality);
		printf("%f\t%d\t%d\n", result.pulse_energy, result.pulse_time, result.quality);
		if(file) fprintf(file, "%d\t%f\t%d\n", result.pulse_time, result.pulse_energy, result.quality);
	}
	return 0;
}

