#include <stdio.h>      // File functions
#include <stdlib.h>     // Memory functions
#include <stdint.h>     // Integer stuff
#include <stdbool.h>    // Boolean crap

bool doubleByte = false;
uint8_t byteFlag = 0;   // Odd or even byte?
uint32_t fileSize = 0;  // Size of the input file
uint32_t offset = 0;    // Input file offset

int main(int argc, char *argv[]) {
    // Check the command line and print usage if there's no input file.
    if(argc < 2) {
        printf("Deinterleave Utility by Ryan Souders\n"
                "Splits a binary file into alternating odd/even files.\n\n"
                "Usage: %s infile [double]\n", argv[0]);
        exit(0);
    }

    /*  This is a lazy hack. If there's a second argument after the file name,
        we double the bit width of the interleaving. This is necessary for some
        ROM files that are interleaved every 16 bits rather than every 8. I'm
        too lazy to write a VA parser here, and this works. Fix it if you want. */
    if(argc == 3) {
        doubleByte = true;
    }

    // Allocate RAM for our output file names
    char *outName1 = (char *)malloc(sizeof(argv[1])+2);
    char *outName2 = (char *)malloc(sizeof(argv[1])+2);

    // Allocate 3 bytes for storage.
    short *curShort = (unsigned short *)malloc(sizeof(unsigned short));
    char *curByte = (char *)malloc(sizeof(char));
    
    // Fill our name buffers
    sprintf(outName1, "%s-even", argv[1]);
    sprintf(outName2, "%s-odd", argv[1]);

    FILE *infile = fopen(argv[1], "rb");    // Input file
    FILE *out1 = fopen(outName1, "wb");     // Output File 1
    FILE *out2 = fopen(outName2, "wb");     // Output File 2

    if(!infile) {
        printf("File read error.\n");
        return -1;
    }

    if((!out1) || (!out2)) {
        printf("File write error.\n");
        return -2;
    }

    fseek(infile, 0, SEEK_SET);     // Make sure we're at the start of the file.
    fseek(infile, 0, SEEK_END);     // Seek to the end.
    fileSize = ftell(infile);       // Get the size.
    rewind(infile);                 // Be kind, rewind.
    
    // Test our output names and print the size in kilobytes.
    printf("Writing %iKB to %s and %s... ", ((fileSize / 2) / 1024), outName1, outName2);

    /*  The basic flow goes something like this:
        Read the data.
        Write the data.
        Increment the offset.
        XOR byteFlag with 1 to flip it.             
        TODO: Add some sanity checks on the fread/fwrite calls. */

    while(offset < fileSize) {                                      // We haven't reached the end yet.
        if(doubleByte) {                                            // Reading 16 bits at a time.
            fread(curShort, sizeof(unsigned short), 1, infile);     // Read 16 bits in
            if(byteFlag == 0) {                                     // Check which file to write to.
                fwrite(curShort, sizeof(unsigned short), 1, out1);  // Write to the even file.
            } else if(byteFlag == 1) {                              // Otherwise...
                fwrite(curShort, sizeof(unsigned short), 1, out2);  // Write to the odd file.
            }
            offset += sizeof(unsigned short);                       // Increment the offset by 16 bits.
        } else {                                                    // Same thing, 8 bits at a time.
            fread(curByte, sizeof(char), 1, infile);
            if(byteFlag == 0) {
                fwrite(curByte, sizeof(char), 1, out1);
            } else if(byteFlag == 1) {
                fwrite(curByte, sizeof(char), 1, out2);
            }
            offset++;
        }
        byteFlag ^= 1;  // XOR byteFlag with 1 to flip it to 1/0.
    }

    // Now that we're done, it's time to clean up.
    // Close all 3 files we opened.
    fclose(infile);
    fclose(out1);
    fclose(out2);

    // Free up the buffers we allocated.
    free(outName1);
    free(outName2);
    free(curByte);
    free(curShort);

    // Tell the user we're done.   
    printf("Done.\n");
    return 0;
}
