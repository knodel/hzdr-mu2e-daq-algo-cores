open_project cutout
set_top findRisingEdge
add_files zero_suppression_cutout_core.cpp
add_files zero_suppression_cutout_core.h
add_files -tb zero_suppression_cutout_test_bench.cpp
open_solution "solution1"
set_part {xc7k420t-ffg1156-2}
create_clock -period 6 -name default

# csim_design -argv "../../../../data/zero_suppression_cutout/input/zero_100_pulses.txt cutout_two.txt"  # change path to the data set here or in zero_suppression_cutout_test_bench.cpp and parameters for that data set in zero_suppression_cutout.h
csynth_design
# cosim_design -rtl verilog -argv "../../../../data/zero_suppression_cutout/input/zero_100_pulses.txt ../../../../cutout_two.txt"  # change path to the data set and output file here or in zero_suppression_cutout_test_bench.cpp and parameters for that data set in zero_suppression_cutout.h
export_design -flow impl

exit
