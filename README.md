# DAQ-Algorithmn-to-FPGA Project

The goal of this project is to analyze detector data from the Stopping Target Monitor (STM) of the Mu2e experiment ([https://mu2e.fnal.gov/](https://mu2e.fnal.gov/)) at Fermilab directly in hardware on the FPGA. The code is developed by **Martin Voigt** and **Rober Ufer** (Supervisors: [Oliver Knodel](mailto:o.knodel@hzdr.de) and Stefan E. Müller) at the Helmholtz-Zentrum Dresden-Rossendorf ([HZDR](https://www.hzdr.de)).

    High Level Synthesis (HLS) is used to optimize the design process and allow non-HDL FPGA programming.

Presentation under Mu2e docdb-ID: 35505

## System Architecture
The overall design process is more focused on a stand-alone hardware core and
a test bench that simulates the flow of data into the core and validates the flow of data out of the
core. A separate CPU on the system is not required or rather not utilized with
the execution of the core itself.

All cores are implemented with `Vivado HLS 2019.1`.

The design is a pure streaming core using `#pragma HLS pipeline` and `hls::stream<>`.

## Algorithms
Currently there are four algorithms:

Algorithm | Implementation | README
---|---|---
Pulse Integratione Quality Analysis <br> (original: Edge-finding Trigger by Jijun Chen, Shihua Huang, Dave Koltick) |  pulse_integration_quality_analysis_core.cpp: <ul><li> [one value per cycle](pulse_integration_quality_analysis/one_value_per_cycle/pulse_integration_quality_analysis_core.cpp) </li><li> [two values per cycle](pulse_integration_quality_analysis/two_values_per_cycle/pulse_integration_quality_analysis_core.cpp) </li></ul> | [README](pulse_integration_quality_analysis/README.md)
Zero Suppression Cutout <br> (original: Zero Suppression by Nam Tran) | zero_suppression_cutout_core.cpp: <ul><li> [one value per cycle](zero_suppression_cutout/one_value_per_cycle/zero_suppression_cutout_core.cpp) </li><li> [two values per cycle](zero_suppression_cutout/two_values_per_cycle/zero_suppression_cutout_core.cpp) </li></ul> | [README](zero_suppression_cutout/README.md)
Moving Window Deconvolution <br> (original: Moving Window Deconvolution by Nam Tran) | MWD.cpp: <ul><li> [one value per cycle](experimental_cores/MWD_one_value_per_cycle/MWD.cpp) </li><li> [two values per cycle](experimental_cores/MWD_two_values_per_cycle/MWD.cpp) </li></ul> | [README](experimental_cores/README.md)
Simple Analysis and Cutout <br> (original: Edge-finding Trigger by Jijun Chen, Shihua Huang, Dave Koltick) | analysis_core.cpp: <ul><li> [two value per cycle](experimental_cores/simple_analysis_and_cutout_core/analysis_core.cpp) </li></ul> | [README](experimental_cores/README.md)
