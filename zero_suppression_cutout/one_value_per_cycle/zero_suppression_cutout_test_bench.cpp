
#include "zero_suppression_cutout_core.h"

int main(int argc, char *argv[]) {
    uint16_t waveform[WF_LENGTH];
    uint16_t data[WF_LENGTH];
    int64_t time[WF_LENGTH];
    uint16_t length;

    //command option evaluation
    std::string in_path = "";
    std::string out_path = "";
    if (argc == 3) {
        printf("using files from command line argument\n");
        in_path = argv[1];
        out_path = argv[2];
    }

    printf("\nImport waveform\n");
    importWaveform(waveform, in_path);

    printf("Print waveform\n");
    printWaveform(waveform, 20);

    printf("Execute HW-Core\n\n");
    executeCore(time, data, waveform, length);

    printf("Print output\n");
    printWindow(time, data, length);

    printf("Write output to file\n");
    writeWindow(time, data, length, out_path);

    printf("\nFinished!\n\n");

    return 0;
}

void importWaveform(uint16_t *waveform, std::string path) {
    FILE *file = fopen(path.c_str(), "r");
    if (file == NULL) {
        perror("Unable to open input file!\n\n");
        exit(1);
    }

    char line[LINE_LENGTH];
    char *pch;
    int i = 0;

    fgets(line, sizeof(line), file);
    pch = strtok(line, " ");

    while ((pch != NULL) && !(i == WF_LENGTH)) {
        sscanf(pch, "%hu", &waveform[i]);
        i++;
        pch = strtok(NULL, " ");
    }
    printf("Imported %d values\n\n", i);
    fclose(file);
}


void printWaveform(uint16_t *waveform, uint16_t length) {
    for (int i = 0; i < length; i++) {
        printf("%d ", waveform[i]);
    }
    printf("\n\n");
}

void executeCore(int64_t *time, uint16_t *data, uint16_t *waveform, uint16_t &length) {
    uint16_t i;
    in_stream_t input_stream;
    out_stream_t output_stream;

    out_stream_data cached_output;

    //write data into input_stream
    for (i = 0; i < WF_LENGTH; i++) {
        input_stream << waveform[i];
    }

    //execute core
    findRisingEdge(input_stream, output_stream);

    //read data from output_stream
    for (i = 0; !output_stream.empty(); i++) {
        cached_output = output_stream.read();
        data[i] = cached_output.data;
        time[i] = cached_output.time;
    }
    length = i;
}

void printWindow(int64_t *time, uint16_t *data, uint16_t length) {
    printf("The length of the output: %d\n\n", length);
    for (int i = 0; i < length; i++) {
        if (data[i] != 0) {
            printf("%ld\t%d\n", time[i], data[i]);
        }
    }
    printf("\n");
}

void writeWindow(int64_t *time, uint16_t *data, uint16_t length, std::string path) {
    FILE *file = fopen(path.c_str(), "w");
    if (file == NULL) {
        perror("Unable to open output file!\n\n");
        exit(1);
    }

    for (int i = 0; i < length; i++) {
        if (data[i] != 0) {
            fprintf(file, "%ld\t%d\n", time[i], data[i]);
        }
    }
    fclose(file);
}
