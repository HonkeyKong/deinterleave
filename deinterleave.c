#include <stdio.h>      // File functions
#include <stdlib.h>     // Memory functions
#include <stdint.h>     // Integer stuff

uint8_t byteFlag = 0;   // Odd or even byte?
uint32_t fileSize = 0;  // Size of the input file
uint32_t offset = 0;    // Input file offset

int main(int argc, char *argv[]) {
    // Check the command line
    if(argc != 2) {
        printf("Deinterleave Utility by Ryan Souders\n"
                "Splits a binary file into alternating odd/even files.\n\n"
                "Usage: %s infile\n", argv[0]);
        exit(0);
    }

    // Allocate RAM for our output file names
    char *outName1 = (char *)malloc(sizeof(argv[1])+2);
    char *outName2 = (char *)malloc(sizeof(argv[1])+2);

    // Allocate a byte for storage.
    char *curByte = (char *)malloc(1);

    // Fill our name buffers
    sprintf(outName1, "%s-0", argv[1]);
    sprintf(outName2, "%s-1", argv[1]);

    FILE *infile = fopen(argv[1], "rb");    // Input file
    FILE *out1 = fopen(outName1, "wb");     // Output File 1
    FILE *out2 = fopen(outName2, "wb");     // Output File 2

    fseek(infile, 0, SEEK_SET);
    fseek(infile, 0, SEEK_END);
    fileSize = ftell(infile);
    rewind(infile);
    
    // Test the size
    printf("Input file is %i bytes.\n", fileSize);
    // Test our output names
    printf("Output files set to %s and %s.\nWriting...\n", outName1, outName2);

    while(offset < fileSize) {
        fread(curByte, 1, 1, infile);
        if(byteFlag == 0) {
            fwrite(curByte, 1, 1, out1);
        } else if(byteFlag == 1) {
            fwrite(curByte, 1, 1, out2);
        }
        byteFlag ^= 1;
        offset++;
    }

    fclose(infile);
    fclose(out1);
    fclose(out2);
    free(outName1);
    free(outName2);
    
    printf("Done.\n");
}