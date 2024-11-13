# Important Types

## TypedUnion and DataType

The `TypedUnion` structure is a cell of data marked with its type. This way, it is possible to store arbitrary data without using the exact type in code, (which would otherwise be necessary, since C is a statically typed language.) The actual type of data can be an integer (`int`), real number (`double`), or null-terminated string (`char *`). A special type is what this library calls "presence". It is used to identify the existence of something (e.g. a command line flag) which does not store an explicit value. The presence or absence of it itself *is* the information.

The type of the value stored in a `TypedUnion` object corresponds to the value of a  `DataType` enum. Those values are `DT_INT`, `DT_DOUBLE`, `DT_STRING` and `DT_PRESENCE`. These identifiers are useful in other parts of the API as well.

`TypedUnion` instances should not be created directly. Insted, factory functions such as `cap_tu_make_presence()` should be used. Similarly, when no longer needed, `TypedUnion`s should be deleted using `cap_tu_destroy()`. Descriptions of these functions is at the end of this section. Keep in mind that, `TypedUnion`s should only be destroyed by the part of the code which "owns" them. The user almost never needs to explicitly create (and own) those objects, and therefore does not directly call the `cap_tu_destroy()` function.

Information can be extracted out of a `TypedUnion` using functions such as `cap_tu_is_double()` (to check if the given object has type `DT_DOUBLE`) or `cap_tu_as_double()` (to return the stored `double` value.)

### Functions
#### cap_tu_make_double
`TypedUnion cap_tu_make_double(double value);`

Creates a new `TypedUnion` of type `double` and stores `value` in it.

#### cap_tu_make_int
`TypedUnion cap_tu_make_int(int value);`

Creates a new `TypedUnion` of type `int` and stores `value` in it.

#### cap_tu_make_presence
`TypedUnion cap_tu_make_presence();`

Creates a new `TypedUnion` of the special "presence" type. This type is used for flags that do not store any explicit value.

#### cap_tu_make_string
`TypedUnion cap_tu_make_string(const char * value);`

Create a new `TypedUnion` of type `string` and stores a copy of `value` in it.

`TypedUnion`s created by this factory contain a dynamically allocated copy
of the string used to create them. The owner of this object should dispose
of it using the function `cap_tu_destroy`. `TypedUnion` objects are usually 
owned by a `ParsedArguments` object so users of the `cap` library do not 
need to call `cap_du_destroy` directly.

#### cap_tu_is_double
#### cap_tu_is_int
#### cap_tu_is_presence
#### cap_tu_is_string

#### cap_tu_as_double
#### cap_tu_as_int
#### cap_tu_as_presence
#### cap_tu_as_string