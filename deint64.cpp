#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>

uint8_t byteFlag = 0;   // Odd or even byte?
uint8_t whichFile = 0;  // Which file are we writing?
uint32_t offset = 0;    // Input file offset
size_t fileSize = 0;    // Size of the input file

// std::ifstream infile;
FILE * infile = NULL;
// std::ofstream out[4];
FILE * out[4] = { NULL, NULL, NULL, NULL };

int main(int argc, char *argv[]) {
    // Check the command line and print usage if there's no input file.
    if(argc < 2) {
        std::cout << "Deinterleave64 Utility by Ryan Souders" << std::endl
        << "Splits a binary file into 4 64-bit alternating files." << std::endl << std::endl
        << "Usage: " << argv[0] << " infile" << std::endl;
        exit(0);
    }

    infile = fopen(argv[1], "rb");

    char *outName[4] = { new char[strlen(argv[1])+2],
                        new char[strlen(argv[1])+2],
                        new char[strlen(argv[1])+2],
                        new char[strlen(argv[1])+2] }; // Length + "-x"

    // Allocate 2 bytes for storage.
    char *curShort = new char[2];
    
    // infile.open(argv[1], std::ios::in|std::ios::binary);

    for(int i = 0; i < 4; i++) {
        sprintf(outName[i], "%s-%i", argv[1], i);
        // out[i].open(outName[i], std::ios::out | std::ios::binary);
        out[i] = fopen(outName[i], "wb");
        // if(!out[i].is_open()) {
        if(!out[i]) {
            std::cout << "File write error." << std::endl;
        }
    }

    // if(!infile.is_open()) {
    if(!infile) {
        std::cout << "File read error." << std::endl;
        return -1;
    }

    // Get the file size.
    // infile.seekg(0, infile.end);
    // fileSize = infile.tellg();
    // infile.seekg(0, infile.beg);

    fseek(infile, 0, SEEK_SET);     // Make sure we're at the start of the file.
    fseek(infile, 0, SEEK_END);     // Seek to the end.
    fileSize = ftell(infile);       // Get the size.
    rewind(infile);                 // Be kind, rewind.
    
    // Print the size in kilobytes.
    std::cout << "Writing " << (fileSize / 1024) << "KB to output files:" << std::endl;
    for(int i = 0; i < 4; i++) std::cout << outName[i] << std::endl;

    /*  The basic flow goes something like this:
        Read the data.
        Write the data.
        Increment the offset.
        Rinse and repeat.   */

    while (offset < fileSize) {
        // infile.read(curShort, sizeof(unsigned short));
        fread(curShort, sizeof(unsigned short), 1, infile);
        // out[whichFile] << curShort;
        fwrite(curShort, sizeof(unsigned short), 1, out[whichFile]);
        offset += sizeof(unsigned short);
        whichFile++;
        if(whichFile > 3) whichFile = 0;
    }
    
    // Now that we're done, it's time to clean up.
    // Close all the files we opened and free up the buffers we allocated.
    // infile.close();
    fclose(infile);

    for(int i = 0; i < 4; i++) {
        // out[i].close();
        fclose(out[i]);
        delete[] outName[i];
    }
    
    delete[] curShort;
    
    // Tell the user we're done.   
    std::cout << "Done." << std::endl;
    return 0;
}
