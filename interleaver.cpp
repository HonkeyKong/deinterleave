#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <iostream>
#include <getopt.h>

enum Mode { DEINTERLEAVE, INTERLEAVE };

void deinterleave(const char* inputFileName, size_t chunkSize, size_t numFiles) {
    FILE* infile = fopen(inputFileName, "rb");
    if (!infile) {
        std::cerr << "File read error: " << inputFileName << std::endl;
        exit(EXIT_FAILURE);
    }

    // Determine the file size
    fseek(infile, 0, SEEK_END);
    size_t fileSize = ftell(infile);
    rewind(infile);

    // Allocate output file names and file pointers
    char** outNames = new char*[numFiles];
    FILE** outFiles = new FILE*[numFiles];
    for (size_t i = 0; i < numFiles; ++i) {
        outNames[i] = new char[strlen(inputFileName) + 3];
        sprintf(outNames[i], "%s-%zu", inputFileName, i);
        outFiles[i] = fopen(outNames[i], "wb");
        if (!outFiles[i]) {
            std::cerr << "File write error: " << outNames[i] << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // Allocate buffer for chunk
    void* buffer = malloc(chunkSize);

    // Deinterleave logic
    size_t offset = 0;
    size_t fileIndex = 0;
    while (offset < fileSize) {
        fread(buffer, chunkSize, 1, infile);
        fwrite(buffer, chunkSize, 1, outFiles[fileIndex]);
        offset += chunkSize;
        fileIndex = (fileIndex + 1) % numFiles;
    }

    // Cleanup
    free(buffer);
    fclose(infile);
    for (size_t i = 0; i < numFiles; ++i) {
        fclose(outFiles[i]);
        delete[] outNames[i];
    }
    delete[] outNames;
    delete[] outFiles;
}

void interleave(const char* outputFileName, char* inputFiles[], size_t chunkSize, size_t numFiles) {
    FILE* outfile = fopen(outputFileName, "wb");
    if (!outfile) {
        std::cerr << "File write error: " << outputFileName << std::endl;
        exit(EXIT_FAILURE);
    }

    // Open input files
    FILE** inFiles = new FILE*[numFiles];
    for (size_t i = 0; i < numFiles; ++i) {
        inFiles[i] = fopen(inputFiles[i], "rb");
        if (!inFiles[i]) {
            std::cerr << "File read error: " << inputFiles[i] << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // Determine the file size (assuming all files are the same size)
    fseek(inFiles[0], 0, SEEK_END);
    size_t fileSize = ftell(inFiles[0]);
    rewind(inFiles[0]);

    // Allocate buffer for chunk
    void* buffer = malloc(chunkSize);

    // Interleave logic
    size_t offset = 0;
    while (offset < fileSize) {
        for (size_t i = 0; i < numFiles; ++i) {
            fread(buffer, chunkSize, 1, inFiles[i]);
            fwrite(buffer, chunkSize, 1, outfile);
        }
        offset += chunkSize;
    }

    // Cleanup
    free(buffer);
    fclose(outfile);
    for (size_t i = 0; i < numFiles; ++i) {
        fclose(inFiles[i]);
    }
    delete[] inFiles;
}

int main(int argc, char* argv[]) {
    int opt;
    Mode mode = DEINTERLEAVE;
    size_t chunkSize = 1;
    size_t numFiles = 2;

    while ((opt = getopt(argc, argv, "dim:c:n:")) != -1) {
        switch (opt) {
            case 'd':
                mode = DEINTERLEAVE;
                break;
            case 'i':
                mode = INTERLEAVE;
                break;
            case 'm':
                if (strcmp(optarg, "deinterleave") == 0) {
                    mode = DEINTERLEAVE;
                } else if (strcmp(optarg, "interleave") == 0) {
                    mode = INTERLEAVE;
                } else {
                    std::cerr << "Invalid mode. Use 'deinterleave' or 'interleave'." << std::endl;
                    exit(EXIT_FAILURE);
                }
                break;
            case 'c':
                chunkSize = atoi(optarg);
                break;
            case 'n':
                numFiles = atoi(optarg);
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " -m <mode> -c <chunk_size> -n <num_files> <input/output files...>" << std::endl;
                exit(EXIT_FAILURE);
        }
    }

    if (mode == DEINTERLEAVE) {
        if (optind >= argc) {
            std::cerr << "Expected input file for deinterleave mode." << std::endl;
            exit(EXIT_FAILURE);
        }
        deinterleave(argv[optind], chunkSize, numFiles);
    } else if (mode == INTERLEAVE) {
        if (optind + numFiles >= argc) {
            std::cerr << "Expected output file and input files for interleave mode." << std::endl;
            exit(EXIT_FAILURE);
        }
        interleave(argv[optind], &argv[optind + 1], chunkSize, numFiles);
    }

    return 0;
}
