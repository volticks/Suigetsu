# Suigetsu

Simple emulator for the MN103 CPU by Panasonic.
Will contain when done:
 - Complete emulation of instruction set.
 - Memory management emulation.
 - Debugging.
 - Detailed logging.

To build:

```sh
mkdir build
cd build 
cmake ../
cmake --build .
```

Usage information can be obtained by running the binary with no arguments:
```
Usage: ./Suigetsu <filepath>
Optional: <program map start> <entrypoint> <load offset> <text section end>
	<program map start>	: Virtual address where program should be loaded.
	<entrypoint>		: PC entrypoint.
	<load offset>		: Offset into the file to start loading the program.
	<text section end>	: Specified end of the text section. If specified will add null byte padding to act as BSS section coming after text section.
Default for all is 0
```

Files and folders:
 - insn\_docs:  Old testing
 - src:         Source code and headers
    - tests:       Files for testing 
    - notes:       Misc notes
