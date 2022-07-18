#include <cstring>
#include <iostream>
#include <fstream>

using namespace std;

bool doubleByte = false;
uint8_t byteFlag = 0;   // Odd or even byte?
uint32_t offset = 0;    // Input file offset
size_t fileSize = 0;    // Size of the input file

ifstream infile;
ofstream out1;
ofstream out2;

int main(int argc, char *argv[]) {
    // Check the command line and print usage if there's no input file.
    if(argc < 2) {
        cout << "Deinterleave Utility by Ryan Souders" << endl
        << "Splits a binary file into alternating odd/even files." << endl << endl
        << "Usage: " << argv[0] << " infile [double]" << endl;
        exit(0);
    }

    /*  This is a lazy hack. If there's a second argument after the file name,
        we double the bit width of the interleaving. This is necessary for some
        ROM files that are interleaved every 16 bits rather than every 8. I'm
        too lazy to write a VA parser here, and this works. Fix it if you want. */
    if(argc == 3) {
        doubleByte = true;
    }

    int i = 0;
    cout << "Args: " << argc << endl;
    for(i = 0; i < argc; i++) cout << argv[i] << endl;

    // Allocate RAM for our output file names
    char *outName1 = new char[strlen(argv[1])+5];   // Length + "-even"
    char *outName2 = new char[strlen(argv[1])+4];   // Length + "-odd"

    // Allocate 3 bytes for storage.
    char *curShort = new char[2];
    char *curByte = new char;
    
    // Fill our name buffers
    sprintf(outName1, "%s-even", argv[1]);
    sprintf(outName2, "%s-odd", argv[1]);

    infile.open(argv[1], ios::in|ios::binary);

    ofstream out1;
    ofstream out2;
    out1.open(outName1, ios::out|ios::binary);
    out2.open(outName2, ios::out|ios::binary);

    if(!infile.is_open()) {
        printf("File read error.\n");
        return -1;
    }

    if((!out1.is_open()) || (!out2.is_open())) {
        printf("File write error.\n");
        return -2;
    }

    infile.seekg(0, infile.end);
    fileSize = infile.tellg();
    infile.seekg(0, infile.beg);
    
    // Test our output names and print the size in kilobytes.
    cout << "Writing " << ((fileSize / 2) / 1024) << "KB to " << outName1 << "and " << outName2 << "... ";

    /*  The basic flow goes something like this:
        Read the data.
        Write the data.
        Increment the offset.
        XOR byteFlag with 1 to flip it.             
        TODO: Add some sanity checks on the fread/fwrite calls. */

    while (offset < fileSize) {
        if(doubleByte) {
            cout << "Reading short..." << endl;
            infile.read(curShort, sizeof(unsigned short));
            if(!byteFlag) {
                // out1.write(curShort, sizeof(unsigned short));
                out1 << curShort;
            } else {
                // out2.write(curShort, sizeof(unsigned short));
                out2 << curShort;
            } offset += sizeof(unsigned short);
        } else {
            cout << "Reading byte..." << endl;
            infile.read(curByte, sizeof(char));
            if(!byteFlag) {
                // out1.write(curByte, sizeof(char));
                out1 << curByte;
            } else {
                // out2.write(curByte, sizeof(char));
                out2 << curByte;
            } offset += sizeof(char);
        } 
        byteFlag ^= 1;
    }
    
    // Now that we're done, it's time to clean up.
    // Close all 3 files we opened.
    infile.close();
    out1.close();
    out2.close();

    // Free up the buffers we allocated.
    delete[] outName1;
    delete[] outName2;
    delete[] curByte;
    delete[] curShort;

    // Tell the user we're done.   
    cout << "Done." << endl;
    return 0;
}
