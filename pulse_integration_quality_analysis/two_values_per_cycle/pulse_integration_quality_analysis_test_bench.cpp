#include "pulse_integration_quality_analysis_core.h"
#include "pulse_integration_quality_analysis_constants.h"

using namespace std;

void show_usage();
PARAMETERS parse_argv(int, char*[]);

int main(int argc, char* argv[]){
	in_stream_t in;
	out_stream_t out;
	out_stream_data_t result;
	in_stream_data_t in_struct;
	int value = 0;

	PARAMETERS p = parse_argv(argc, argv);
	if(!p.valid) return 1;
	cout << "\nPARAMETERS:" << endl;
	cout << " delta_PF: "          << p.delta_PF          << endl;
	cout << " delta_Integration: " << p.delta_Integration << endl;
	cout << " delta_Gap: "         << p.delta_Gap         << endl;
	cout << " delta_PB: "          << p.delta_PB          << endl;
	cout << " delta_Walkback: "    << p.delta_Walkback    << endl;
	cout << endl;
	cout << " one_mV: "            << p.one_mV            << endl;
	cout << " threshold: "         << p.threshold         << endl;
	cout << " bound2: "            << p.bound2            << endl;
	cout << " bound1: "            << p.bound1            << endl;
	cout << endl;
	cout << " output: "            << p.output            << endl;
	cout << " input: "             << p.input             << endl;
	cout << " quiet: "             << p.quiet             << endl;

	printf("\nEXECUTION:\n");
	FILE* file = fopen(p.input.c_str(), "r");
	if(file == NULL){
		perror(" Unable to open input file! (at --input option)");
		return 1;
	}
	printf(" file opened\n");

	//read file
	for(int i=0; i<data_len; i++){
		if(fscanf(file, "%d", &value) != 1) printf(" unable to read line %d\n", i);
		if(i%2 == 0) {
			in_struct.first_data = value;
		}
		else{
			in_struct.second_data = value;
			in << in_struct;
		}
	}
	fclose(file);
	printf(" data read\n");

	//execute core
	analysis_core(in, out,
			p.delta_Walkback, p.delta_PF, p.delta_Integration, p.delta_Gap, p.delta_PB,
			p.threshold, p.one_mV, p.bound2, p.bound1);
	printf(" core executed\n");

	//print results
	file = NULL;
	if(p.output != "no") file = fopen(p.output.c_str(), "w");
	if(file == NULL) printf(" file output disabled\n");
	else printf(" initiated file output\n");

	printf("\nRESULTS:\n");
	int count_output = 0;
	while(!out.empty()){
		out >> result;
		count_output += 1;
		//if(!p.quiet || count_output == 1) printf("   energy: %f, time: %d, quality: %d\n", result.pulse_energy, result.pulse_time, result.quality);
		if(!p.quiet || count_output == 1) printf(" %ld\t%f\t%d\n", result.pulse_time, result.pulse_energy, result.quality);
		if(file) fprintf(file, "%ld\t%f\t%d\n", result.pulse_time, result.pulse_energy, result.quality);
	}
	if(file) fclose(file);
	if(p.quiet && count_output > 0) printf(" ... %d results not printed\n", count_output-1);
	printf("\n");
	return 0;
}

void show_usage(){
	cout << "options:" << endl;
	cout << " -h / --help     print this message" << endl;
	cout << " -q              don't print all results" << endl;
	cout << endl;
	cout << " delta_PF + delta_Walkback may not exceed 60 (see max_trigger_delay in analysis_core.h to change this limit)" << endl;
	cout << " delta_* parameters expressed in number of data points taken" << endl;
	cout << " --delta_PF          [int] default=" << delta_PF         << "  length of time of front pedestal (baseline)" << endl;
	cout << " --delta_Integration [int] default=" << delta_Integration << " pulse integration time, starting from walkback point" << endl;
	cout << " --delta_Gap         [int] default=" << delta_Gap        << "  unused time gap to allow complete pulse decay" << endl;
	cout << " --delta_PB          [int] default=" << delta_PB         << "  length of time of back pedestal (baseline)" << endl;
	cout << " --delta_Walkback    [int] default=" << delta_Walkback   << "  time required to account for pulse rising edge before trigger" << endl;
	cout << endl;
	cout << " --one_mV         [double] default=" << one_mV           << "  the number representing one mV in the unit of input values" << endl;
	cout << " --threshold      [double] default=" << threshold        <<  " trigger occurs if geometric mean of two consecutive differences accede threshold (in mV)" << endl;
	cout << " --bound2         [double] default=" << bound2           << "  threshold for quality=2" << endl;
	cout << " --bound1         [double] default=" << bound1           << "  threshold for quality=1" << endl;
	cout << endl;
	cout << " --output         [string] default=" << output           << "  path to output file" << endl;
	cout << " --input          [string] default=" << input            << "  path to input file of 16 bit integers" << endl;
}

PARAMETERS parse_argv(int argc, char* argv[]){
	PARAMETERS p;

	//default values
	p.delta_PF = delta_PF;
	p.delta_Integration = delta_Integration;
	p.delta_Gap = delta_Gap;
	p.delta_PB = delta_PB;
	p.delta_Walkback = delta_Walkback;

	p.one_mV = one_mV;
	p.threshold = threshold;
	p.bound2 = bound2;
	p.bound1 = bound1;

	p.output = output;
	p.input = input;
	p.quiet = false;

	p.valid = false;

	//parse loop
	for(int i = 1; i < argc;){
		std::string arg = argv[i++];
		if(i >= argc && arg != "-h" && arg != "--help" && arg != "-q"){cout << "[OPTION WITHOUT VALUE]: " << arg << endl; show_usage(); return p; }

		try{
			if(arg == "-h" || arg == "--help"){ show_usage(); return p; }

			else if(arg == "--delta_PF")          p.delta_PF          = stoi(argv[i++]);
			else if(arg == "--delta_Integration") p.delta_Integration = stoi(argv[i++]);
			else if(arg == "--delta_Gap")         p.delta_Gap         = stoi(argv[i++]);
			else if(arg == "--delta_PB")          p.delta_PB          = stoi(argv[i++]);
			else if(arg == "--delta_Walkback")    p.delta_Walkback    = stoi(argv[i++]);

			else if(arg == "--one_mV")            p.one_mV            = stod(argv[i++]);
			else if(arg == "--threshold")         p.threshold         = stod(argv[i++]);
			else if(arg == "--bound2")            p.bound2            = stod(argv[i++]);
			else if(arg == "--bound1")            p.bound1            = stod(argv[i++]);

			else if(arg == "--output")            p.output            = string(argv[i++]);
			else if(arg == "--input")             p.input             = string(argv[i++]);
			else if(arg == "-q")                  p.quiet             = true;

			else {cout << "[INVALID OPTION]: " << arg << endl; show_usage(); return p; }
		}
		catch(const std::exception &e){cout << "[ERROR]: " << e.what() << " (at " << arg << ")" << endl; show_usage(); return p; }
	}

	//no error occurred
	p.valid = true;
	return p;
}

