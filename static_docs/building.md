# Building the Library

The `cap` library is used as a single header file `cap.h`. However, the library
code is placed in multiple units inside the `headers/` directory which need to
be combined to create that file. This is done using a program called `slicer`
included in the project. The provided `Makefile` can be used to build `slicer`
and then use it to create the library file, see the following example.
``` console
$ make all
gcc -Wall -Wextra -Wformat-security -pedantic -std=c99 -g -I. -o slicer.exe slicer.c
./slicer.exe -o cap.h headers/data_type.h [... sevaral other file names here...]
```
