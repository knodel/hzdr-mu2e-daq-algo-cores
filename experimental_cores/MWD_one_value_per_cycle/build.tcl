open_project MWD1
set_top mwd
add_files MWD.cpp
add_files MWD.h
add_files -tb tb_MWD.cpp
open_solution "solution1"
set_part {xc7k420t-ffg1156-2}
create_clock -period 6 -name default

# csim_design -argv "../../../../data/core/input/dataset.txt"  # change path to the data set here or in tb_MWD.cpp and parameters for that data set in MWD.h
csynth_design
# cosim_design -argv "../../../../data/core/input/dataset.txt"  # change path to the data set here or in tb_MWD.cpp and parameters for that data set in MWD.h
export_design -flow impl

exit
