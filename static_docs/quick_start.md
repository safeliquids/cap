# Quick Start

## Compiling

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

## Importing `cap.h` and Creating a Parser

To use the library, we first include the generated `cap.h` in the program's 
main source file. *(Note: `cap.h` must not be included in multiple compilation
units. This is a technical limitation caused by it being a single header. In 
future the library will be changed to consist of one header and one source file
which the user can compile to a single object file. This will allow the library 
to be used in multiple compilation units.)*
``` c
/* main.c */
#include "cap.h"
...
```
Then in the main function, we can create a parser object.
``` c
...
int main (int argc, const char ** argv) {
    ArgumentParser * parser = cap_parser_make_default();
    ...
}
```

## Defining Flags

We can define flags for our program like so 
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
once. We don't need to pay attention to the last two parameters for now.

Flags can not only take `double` values. We can use `DT_INT` instead of 
`DT_DOUBLE` for integers, or `DT_STRING` for literal strings. If the flag should
take no value, we use a special key `DT_PRESENCE`.

The maximum number of times a specific flag may be present on the 
command line (the second number in the example above) must naturally be at
least the minimum (the first number). To allow a flag up to any number 
of times, we can use `-1` instead of a positive integer, like so.
``` c
    /* inside main() */
    ...
    cap_parser_add_flag(parser, "-x", DT_STRING, 0, -1, NULL, NULL);
    ...
```

## Defining Positional Arguments

In addition to flags, positional arguments (or 'positionals') can 
also be configured for our program. See this example.
``` c
    /* inside main() */
    ...
    cap_parser_add_positional(
        parser, "filename", DT_STRING, true, false, NULL, NULL);
    ...
```
This call signature should look somewhat familiar - the positional argument's 
name is "filename" and its value is a string. The two boolean parameters
indicate that it is required (`true`) and that it is not variadic (`false`).
Once again, we don't need to care about the last two arguments for now.

Arguments defined this way are positional, which means they must appear on the 
command line in the same order as they were defined. In contrast, flags and
their values can appear in any order, and may even be mixed inbetween
positionals. Since we defined the argument as required and non-variadic, it must
be found on the command line when parsing, and takes exactly one value.

## Finally, Parsing

After we have defined all necessary flags and positionals for our program, we
are ready to finally process the words given on the command line
``` c
    /* inside main() */
    ... 
    ParsedArguments * parsed_args = cap_parser_parse(parser, argc, argv);
    ...
```
The `ParsedArguments` contains information about flags and positional that
were found on the command line. We can use library functions to get some 
information from it.

## Accessing Parsed Flags

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
``` c
    /* inside main() */
    ...
    if (!cap_tu_is_double(big_value)) {
        printf("oh no, it was not double!\n");
	exit(-1);
    }
```
To extract the actual number from `big_value`, we can do this
``` c
    /* inside main() */
    ...
    double big_number = cap_tu_as_double(big_value);
    ...
```

## Accessing Parsed Positionals

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

## Errors

Most errors related to the parser cause the program to exit with an error
message. For example, when trying to create a flag that does not start with
a flag prefix character ('-' by default), it may result in something like
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

## I Don't Want to Play With You Any More!

Once a parser object is no longer needed, it can be deleted using the function
`cap_parser_destroy`. Any `ParsedArguments` object returned from 
`cap_parser_parse` should be deleted using `cap_pa_destroy`. Values retrieved
from a `ParsedArguments` (such as using `cap_pa_get_flag`) are still owned by 
that object. That way they do not need to be deleted by the user, however, they 
*must not* be used after the `ParsedArguments` is  deleted.