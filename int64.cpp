#include <cstdio>
#include <cstring>
#include <iostream>
// #include <fstream>

uint8_t whichFile = 0;  // Which file are we reading?
uint32_t offset = 0;    // Input file offset
size_t fileSize = 0;    // Size of the input file

FILE * outfile = NULL;
FILE * in[4] = { NULL, NULL, NULL, NULL };

int main(int argc, char *argv[]) {
    // Check the command line and print usage if there's no input file.
    if(argc < 6) {
        std::cout << "Interleave64 Utility by Ryan Souders" << std::endl
        << "Stitches 4 64-bit alternating files together." << std::endl << std::endl
        << "Usage: " << argv[0] << " outfile infile1 infile2 infile3 infile4" << std::endl;
        exit(0);
    }

    outfile = fopen(argv[1], "wb");

    char *inName[4] = {
        argv[2],
        argv[3],
        argv[4],
        argv[5]
    };

    // Allocate 2 bytes for storage.
    char *curShort = new char[2];

    for(int i = 0; i < 4; i++) {
        in[i] = fopen(inName[i], "rb");
        if(!in[i]) {
            std::cout << "File read error: " << inName[i] << std::endl;
        }
    }

    if(!outfile) {
        std::cout << "File write error." << std::endl;
        return -1;
    }

    fseek(in[0], 0, SEEK_SET);     // Make sure we're at the start of the file.
    fseek(in[0], 0, SEEK_END);     // Seek to the end.
    fileSize = ftell(in[0]);       // Get the size.
    rewind(in[0]);                 // Be kind, rewind.
    
    // Print the size in kilobytes.
    std::cout << "Writing " << (fileSize / 1024) << "KB to output file " << argv[1] << "..." << std::endl;

    /*  The basic flow goes something like this:
        Read the data.
        Write the data.
        Loop back through each file.
        Increment the offset.
        Rinse and repeat.   */

    while (offset < fileSize) {
        for(int i = 0; i < 4; i++) {
            fread(curShort, sizeof(unsigned short), 1, in[i]);
            fwrite(curShort, sizeof(unsigned short), 1, outfile);
        }
        offset += sizeof(unsigned short);
    }
    
    // Now that we're done, it's time to clean up.
    // Close all the files we opened and free up the buffers we allocated.
    // infile.close();
    fclose(outfile);

    for(int i = 0; i < 4; i++) {
        // out[i].close();
        fclose(in[i]);
        delete[] inName[i];
    }
    
    delete[] curShort;
    
    // Tell the user we're done.   
    std::cout << "Done." << std::endl;
    return 0;
}
