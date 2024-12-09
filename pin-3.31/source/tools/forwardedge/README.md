# Forward Edge Control Flow Integrity (CFI) Pintool

This project implements a Forward Edge Control Flow Integrity using the Intel PIN tool framework. It ignores indirect calls outside of [low,high]. If the target is in the range [low,high], it should be allowed if the target address is in file f.

Make sure to copy the entire contents of this folder and put it in the following path:

``` bash
<path to pin tool>/source/tools
```

## Usage
1. **Build the Pintool**:
   Compile the tool using the Intel PIN build system.

   ```bash
   make
   ```

   or 

   ```bash
   makefile
   ```

2. **Run the Pintool**:
   Use the PIN tool to execute the application with the configured parameters.

   ```bash
   pin -t <path_to_pintool> -low <l> -high <h> -f <file> -- <application>
   ```

    Low represents the lower limit and high represents the upper limit.

3. **Test Invalid Scenarios**:
   Does not work for firefox, readelf, soffice


