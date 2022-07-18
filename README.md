# deinterleave

Really, there's not much to say. It's a tool for deinterleaving files into their odd/even pairs. I wrote it for arcade ROM hacking purposes, but I'm sure it'll also come in handy for other M68K-based systems, and platforms that I couldn't think of if I tried.

For the uninitiated, a lot of arcade ROMs are interleaved, with the even bytes on one ROM chip, and the odd bytes on another. This is basically the reverse implementation of the [interleave](https://github.com/HonkeyKong/interleave) tool, in the sense that instead of combining the two files together, it splits them apart into odd and even pairs for re-insertion into your EEPROMs or whatever your evil plans may be.

There's no makefile, but it's only one source file, so you just have to type:  
`gcc -o deinterleave deinterleave.c`

And the program is built and ready to use. For the default 8-bit deinterleaving, run the program like so:  
`./deinterleave infile`

To deinterleave files 16 bits at a time, run it like so:  
`./deinterleave infile literallyanything`

Just put anything after the input file. I was too lazy to write a VA parser for the one option in this program.