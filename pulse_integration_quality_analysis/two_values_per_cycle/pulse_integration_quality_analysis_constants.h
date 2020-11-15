#include <stdio.h>
#include <string>
#include <iostream>


//**PARAMETERS TO BE DETERMIND**
//delta_PF + delta_Walkback may not exceed 60 (see max_trigger_delay in analysis_core.h to change this limit)
//delta_* parameters expressed in number of data points taken
const int delta_PF = 32;           //length of time of front pedestal (baseline)
const int delta_Integration = 128; //pulse integration time, starting from walkback point
const int delta_Gap = 16;          //unused time gap to allow complete pulse decay
const int delta_PB = 32;           //length of time of back pedestal (baseline)
const int delta_Walkback = 16;     //time required to account for pulse rising edge before trigger

const double one_mV = 400;         //the number representing one mV in the unit of input values
const double threshold = 0.97656;  //trigger occurs if geometric mean of two consecutive differences accedes threshold (in mV)
const double bound2 = 25;          //threshold for quality=2
const double bound1 = 50;          //threshold for quality=1

//**JUST FOR TEST BENCH**
const std::string input = "";
const std::string output = "no";


//**STRUCT TO HOLD THESE PARAMETERS**
struct PARAMETERS{
	int delta_PF;
	int delta_Integration;
	int delta_Gap;
	int delta_PB;
	int delta_Walkback;

	double one_mV;
	double threshold;
	double bound2;
	double bound1;

	std::string output;
	std::string input;
	bool quiet;

	bool valid;
};

