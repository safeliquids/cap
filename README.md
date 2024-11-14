# cap 

`cap.h` is a C library for parsing arguments given to programs on the command
line. Its structure and API takes great inspiration from `argparse` [1], a part
of the standard library of python 3.

The `cap` library is used in the form of an amalgamated header file that can be
included like any other regular header. To learn how to create this amalgamation,
visit [Building](./static_docs/building.md). For a simplified guide on how to
use the library, visit [the Quick Start page](./static_docs/quick_start.md).

*Note: some of this documentation is very brief and does not contain all useful
information. Extending the documentation is a current work in progress. Most
code in the `headers` directory is documented, but keep in mind that, some
things are not explained there, and not all documented parts of the codebase
should be directly interacted with.*

## References

[1] Python Software Foundation. *`argparse` - Parser for command-line options, arguments and sub-commands.* Version 3.12.7. Online. (cit. 5. 10. 2024) [https://docs.python.org/3.12/library/argparse.html](https://docs.python.org/3.12/library/argparse.html)
