# cap {#mainpage}

*Note: this documentatin is still a work in progress. Some functionality is not
properly documented, documentation is sometimes not well structured, and not all
documented code should be directly used.

Cap is a C Argument Parser library which allows parsing flags and arguments
given to programs on the command line. This library is used as a single header
file to be `#include`d in a main program unit. Great inspiration in the API is
taken from the `argparse` module in the Python standard library [1].

## Topics

This documentation is structured in several topic pages. 
- [Quick Start](./quick_start.md) contains a brief overview of how to use the
  library.
- Check out [Building](./building.md) for more information on how the library is
  built.
- [Data Types](@ref typed_union) and [Parsed Arguments](@ref parsed_arguments)
  describes how data is stored in the library.
- [Argument Parser](@ref parser) describes the `ArgumentParser` object which
  constitues the API for defining arguments and is used for parsing.

## References

[1] Python Software Foundation. *`argparse` - Parser for command-line options, arguments and sub-commands.* Version 3.12.7. Online. (cit. 5. 10. 2024) [https://docs.python.org/3.12/library/argparse.html](https://docs.python.org/3.12/library/argparse.html)
