open_project simple_analysis_core
set_top analysis_core
add_files analysis_core.cpp
add_files analysis_core.h
add_files -tb analysis_test.cpp
open_solution "solution1"
set_part {xc7k420t-ffg1156-2}
create_clock -period 6 -name default

# csim_design  # change path to data set in analysis_test.cpp and parameters for that data set in analysis_core.h
csynth_design
# cosim_design  # change path to data set in analysis_test.cpp and parameters for that data set in analysis_core.h
export_design -flow impl

exit
