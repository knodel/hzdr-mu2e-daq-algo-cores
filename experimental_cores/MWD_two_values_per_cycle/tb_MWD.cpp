#include <iostream>
#include <fstream>
#include "MWD.h"

using namespace std;

int main(int argc, char* argv[]){
	fstream file;
	double tmp_in;
	report tmp_out;
	in_stream_t in_stream;
	out_stream_t out_stream;
	in_stream_data_t input_element;

	//open file
	if(argc >= 2) file.open(argv[1]);
	else file.open("");  // insert path to data set here
	if(!file.is_open()){
		cout << "file not found" <<endl;
		return 1;
	}

	//read input
	int pulse_count = 0;
	while(file >> tmp_in){
		//read first value
		input_element.first_value = tmp_in;
		input_element.second_value = tmp_in;
		input_element.end_of_pulse = true; // gets overwritten if second value available

		//read second value
		if(file.peek() != '\n' && file >> tmp_in){
			input_element.second_value = tmp_in;
			input_element.end_of_pulse = file.peek() == '\n';
		}

		//write to input stream
		in_stream << input_element;
		if(file.peek() == '\n' && ++pulse_count == max_pulse_count) break;

	}
	file.close();

	//run core
	mwd(in_stream, out_stream);

	//write output
	while(!out_stream.empty()){
		out_stream >> tmp_out;
		cout << tmp_out.time << '\t' << tmp_out.height << endl;
	}

	cout << "finished" << endl;
	return 0;
}
