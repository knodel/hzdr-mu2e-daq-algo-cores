open_project analysis_core
set_top analysis_core
add_files pulse_integration_quality_analysis_core.cpp
add_files pulse_integration_quality_analysis_core.h
add_files -tb pulse_integration_quality_analysis_test_bench.cpp
open_solution "solution1"
set_part {xc7k420t-ffg1156-2}
create_clock -period 6 -name default

# csim_design -argv "../../../../data/pulse_integration_quality_analysis/input/LaBr_dataset.txt"  # change path to the data set here or in pulse_integration_quality_analysis_test_bench.cpp and parameters for that data set in pulse_integration_quality_analysis_core.h
csynth_design
# cosim_design -argv "../../../../data/pulse_integration_quality_analysis/input/LaBr_dataset.txt ../../../../analysis_one.txt"  # change path to the data set here or in pulse_integration_quality_analysis_test_bench.cpp and parameters for that data set in pulse_integration_quality_analysis_core.h
export_design -flow impl

exit
