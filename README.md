# cap

`cap.h` is a C library for parsing arguments given to programs on the command
line. Its structure and API takes great inspiration from `argparse` [1], a part
of the standard library of python.

## Quick Start

### Compiling

You can use the provided `Makefile` to create a header file called `cap.h`. 
``` console
$ make all
gcc -Wall -Wextra -Wformat-security -pedantic -std=c99 -g -I. -o slicer.exe slicer.c
./slicer.exe -o cap.h headers/types.h [... sevaral other file names here...]
```
As you can see above, this involves building a tool called `slicer.exe`, whose
source code can be found in the file `slicer.c` in this repository. The default 
toolchain to do this is `gcc`, you can edit the `Makefile` to use your preferred
C compiler.

### Importing `cap.h` and Creating a Parser

To use the library, include `cap.h` in your main program file.
``` c
/* main.c */
#include "cap.h"
...
```
Then, in the main function, create a parser object.
``` c
...
int main (int argc, const char ** argv) {
    ArgumentParser * parser = cap_parser_make_default();
    ...
}
```

### Defining Flags

You can define flags for your program like so 
``` c
    /* inside main() */
    ...
    cap_parser_add_flag(parser, "--big", DT_DOUBLE, 0, 1, NULL, NULL);
    ...
```
The above example creates a flag called "--big" that must be followed by
a real number on the command line. That number becomes the value of this flag
and is stored as a `double`, hence the `DT_DOUBLE`. The two numbers following
`DT_DOUBLE` specify the minimum and maximum number of times this flag is
allowed to be on the command line - in this case the flag may be present up to
once. Don't worry about the last two parameters for now.

Flags can not only take `double` values. You can use `DT_INT` instead of 
`DT_DOUBLE` for integers, or `DT_STRING` for literal strings. If the flag should
take no value, use a special key `DT_PRESENCE`.

The maximum allowed number of times a specific flag may be present on the 
command line (the second number in the example above) must naturally be at
least the minimum number (the first number.) To allow a flag up to any number 
of times, use `-1` instead of a positive number, like so.
``` c
    /* inside main() */
    ...
    cap_parser_add_flag(parser, "-x", DT_STRING, 0, -1, NULL, NULL);
    ...
```

### Defining Positional Arguments

In addition to flags, you can also configure positional arguments (or 
positionals, for short) for your program. See this example
``` c
    /* inside main() */
    ...
    cap_parser_add_positional(parser, "filename", DT_STRING, NULL, NULL);
    ...
```
This call signature should look somewhat familiar - the positional argument's 
name is "filename" and its value is a string. Once again, we don't need to care
about the function's last two arguments for now.

Arguments defined this way are positional, which means they must appear on the 
command line in the same order as they were defined. Flags and their values can
appear in any order, and may even be mixed inbetween positionals.
Positional arguments are always required and must be found exactly once.

### Finally, Parsing

After we have defined all necessary flags and positionals for our program, we
are ready to finally process the words given on the command line
``` c
    /* inside main() */
    ... 
    ParsedArguments * parsed_args = cap_parser_parse(parser, argc, argv);
    ...
```

### The `ParsedArgument` Object

The `ParsedArgument` contains information about flags and positional that
were found on the command line. We can use library functions to get some 
information from it.
 
#### Flags

For example, to check if a flag has been found
on the command line, we can use something like this
``` c
    /* inside main() */
    ...
    if (cap_pa_has_flag(parsed_args, "-x")) {
	do_something_because_x_was_given();
    }
    ...
```
Before, we defined the "-x" flag so that it may be given
any number of times. To check the number of times it was found we can use this
``` c
    /* inside main() */
    ...
    size_t x_count = cap_pa_flag_count(parsed_args, "-x");
    ...
```

To extract values stored for flags, we use this
``` c
    /* inside main() */
    ...
    if (!cap_pa_has_flag(parsed_args, "--big") {
        do_something_when_big_is_absent();
	exit(-1);
    }
    
    const TypedUnion * big_value = cap_pa_get_flag(parsed_args, "--big");
    ...
```
As the type name suggests, this function returns an object that contains a 
value and information about that value's type. We have previously defined the 
"--big" flag to take a real number for a value. Although the parser should 
never store an incorrect type, we can check if the value 
stored in `parsed_args` is indeed a `DT_DOUBLE`
```
    /* inside main() */
    ...
    if (!cap_tu_is_double(big_value)) {
        printf("oh no, it was not double!\n");
	exit(-1);
    }
```
To extract the actual number from `big_value`, we can do this
```
    /* inside main() */
    ...
    double big_number = cap_tu_as_double(big_value);
    ...
```

#### Positionals

In a similar fashion, we can query if `parsed_args` contains a positional
argument.
``` c
    /* inside main() */
    ...
    if (!cap_pa_has_positional(parsed_args, "filename")) {
	do_something_because_no_filename();
	exit(-1);
    }
    ...
```
Other operations regarding positionals are also quite similar.
``` c
    /* inside main() */
    ...
    const TypedUnion * fil = cap_pa_get_positional(parsed_args, "filename");
    const char * filename = cap_tu_as_string(fil);
    ...
```

### Errors

Most errors related to the parser cause the program to exit with an error
message. For example, when trying to create a flag that does not start with
a flag prefix character ('-' by default), you may see something like
``` console
$ ./myprogram
cap: invalid flag name - must begin with one of "-"
```
Similar messages will appear for other errors connected to configuring 
a parser.

Another kind of error can happen at parse time. These will also crash the 
program. For example, unknown flags wil cause something like
``` console
$ ./myprogram -P
myprogram: unknown flag '-P'

usage:
        myprogram [-h] [--big DOUBLE] [-x STRING] [--] <filename>
```

### I Don't Want to Play With You Any More!

Once a parser object is no longer needed, it can be deleted using the function
`cap_parser_destroy`. Any `ParsedArguments` object returned from 
`cap_parser_parse` should be deleted using `cap_pa_destroy`. Values retrieved
from a `ParsedArguments` (such as using `cap_pa_get_flag`) are still owned by 
that object. That way they do not need to be deleted by the user but they 
may not be used after the `ParsedArguments` is  deleted.

## References

[1] Python Software Foundation. *`argparse` - Parser for command-line options, arguments and sub-commands.* Version 3.12.7. Online. (cit. 5. 10. 2024) (https://docs.python.org/3.12/library/argparse.html)[https://docs.python.org/3.12/library/argparse.html]
