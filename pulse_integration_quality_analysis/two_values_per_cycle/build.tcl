open_project analysis_core
set_top analysis_core
add_files pulse_integration_quality_analysis_core.cpp
add_files pulse_integration_quality_analysis_core.h
add_files -tb pulse_integration_quality_analysis_test_bench.cpp
add_files -tb pulse_integration_quality_analysis_constants.h
open_solution "solution1"
set_part {xc7k420t-ffg1156-2}
create_clock -period 6 -name default

# csim_design -argv "--input ../../../../data/pulse_integration_quality_analysis/input/LaBr_int16.txt"  # set parameters here (--help for parameter names) set length of data (data_len) in pulse_integration_quality_analysis_core.h
csynth_design
# cosim_design -rtl verilog -argv "--input ../../../../data/pulse_integration_quality_analysis/input/LaBr_int16.txt --output ../../../../analysis_two.txt"
export_design -flow impl

exit
