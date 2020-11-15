# Zero Suppression Cutout
Algorithm | Implementation | Description
---|---|---
Zero Suppression Cutout <br> (original: Zero Suppression by Nam Tran) | zero_suppression_cutout_core.cpp: <ul><li> [one value per cycle](one_value_per_cycle/zero_suppression_cutout_core.cpp) </li><li> [two values per cycle](two_values_per_cycle/zero_suppression_cutout_core.cpp) </li></ul> | <ul><li> Calculation of standard deviation and mean with sliding window </li><li> Find rising edge (mean > standard deviation * threshold) </li><li> Return data around trigger </li></ul>

## Interfaces
* **Input for test bench:** 16 bit unsigned integers (file with one pulse per line, values separated by one space)

### One Input Value Per Clock Cycle Implementation
* **Input:** One stream of 16 bit unsigned integers
* **Output:** Two streams (one 64 bit integers time and one 16 bit unsigned integers cut out data)

### Two Input Values Per Clock Cycle Implementation
* **Input:** Two streams of 16 bit unsigned integers
* **Output:** Four streams (two 64 bit integers time and two 16 bit unsigned integers cut out data)

Block Diagram | Stream Diagram
---|---
![~Zero Suppression Cutout Block~](img/zero_suppression_cutout_block.png  "Zero Suppression Cutout Block") | ![~Zero Suppression Cutout Stream~](img/zero_suppression_cutout_stream.svg  "Zero Suppression Cutout Stream")

## Visualization
### Mean and Standard deviation of one Pulse
![~Zero Suppression Cutout Data~](img/zero_suppression_cutout_data.svg  "Zero Suppression Cutout Data")

### Trigger Condition
![~Zero Suppression Cutout Trigger~](img/zero_suppression_cutout_trigger.svg  "Zero Suppression Cutout Trigger")

## Resource Usage
The FPGA at the STM experiment at Fermilab is a Xilinx Kintex-7 `xc7k420t-ffg1156-2`.

FPGA | Total SLICE | Total LUT | Total FF | Total DSP | Total BRAM
---|---|---|---|---|---
Kintex-7 (xc7k420t-ffg1156-2) | 74650 | 260600 | 521200 | 1680 | 835

CP required: 6.000 <br>
Uncertainty: default (12.5%)

Algorithm | Optimized | Timing in ns | SLICE | LUT | FF | DSP | BRAM | Time per Pulse in ns
---|---|---|---|---|---|---|---|---
One value per clock cycle | yes | 5.098 | 924 | 2490 | 2655 | 27 | 2 | 5194.862
Two values per clock cycle | yes | 5.298 | 2109 | 5051 | 6329 | 74 | 2 | 2699.331

![~Resource Usage in Percentage~](img/xc7k420t_ffg1156_2_zero_suppression_cutout.svg  "Resource Usage in Percentage")
