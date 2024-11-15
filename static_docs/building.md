# Building the Library

*Note: this page is a work in progress. Expect more comprehensive information
soon.*

The provided `Makefile` can be used to create a header file called `cap.h`. 
``` console
$ make all
gcc -Wall -Wextra -Wformat-security -pedantic -std=c99 -g -I. -o slicer.exe slicer.c
./slicer.exe -o cap.h headers/types.h [... sevaral other file names here...]
```
As shown above, this involves building a tool called `slicer.exe`, the source 
code of which can be found in the file `slicer.c` in this repository. The default 
toolchain to do this is `gcc`, you can edit the `Makefile` to use your preferred
C compiler.
