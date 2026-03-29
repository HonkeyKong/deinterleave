# interleaver

`interleaver` is a small command-line utility for working with interleaved binary data.
It currently supports three operations:

- `deinterleave`: split one input file across multiple output files in fixed-size chunks
- `interleave`: combine multiple input files into one output file in fixed-size chunks
- `byteswap`: swap every pair of bytes in a file and write the result to a new file

This project is aimed at ROM and binary data workflows, but it can be used anywhere chunked interleaving or 16-bit byte swapping is useful.

## Build

There is no build system in the repo right now. Compile `interleaver.cpp` directly:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic interleaver.cpp -o interleaver
```

## Usage

```text
./interleaver -m deinterleave -c <chunk_size> -n <num_files> <input_file>
./interleaver -m interleave   -c <chunk_size> -n <num_files> <output_file> <input_files...>
./interleaver -m byteswap <input_file> <output_file>
```

The short flags `-d` and `-i` still select deinterleave and interleave modes respectively. `-m` is the only way to select `byteswap`.

## Modes

### Deinterleave

Reads one input file and writes alternating fixed-size chunks into numbered output files named `<input_file>-0`, `<input_file>-1`, and so on.

Example, split a file into even/odd bytes:

```bash
./interleaver -m deinterleave -c 1 -n 2 program.bin
```

Example, split a file into 16-bit words across two files:

```bash
./interleaver -m deinterleave -c 2 -n 2 program.bin
```

Example, split a file round-robin across four outputs in 2-byte chunks:

```bash
./interleaver -m deinterleave -c 2 -n 4 data.bin
```

### Interleave

Reads `num_files` inputs and writes one chunk from each input in sequence into the output file.

Example, combine odd/even byte files back into one file:

```bash
./interleaver -m interleave -c 1 -n 2 merged.bin program.bin-0 program.bin-1
```

Example, combine four files using 2-byte chunks:

```bash
./interleaver -m interleave -c 2 -n 4 merged.bin part0.bin part1.bin part2.bin part3.bin
```

### Byteswap

Reads one input file, swaps each adjacent byte pair, and writes the swapped result to a new output file.

Example:

```bash
./interleaver -m byteswap rom.bin rom-swapped.bin
```

An input sequence like:

```text
12 34 56 78
```

becomes:

```text
34 12 78 56
```

`byteswap` currently requires an even-sized input file.

## Notes

- `interleave` assumes all input files are the same size.
- The tool currently performs minimal validation on chunk sizing and file lengths, so it is best used with inputs that are already known to match the intended layout.
