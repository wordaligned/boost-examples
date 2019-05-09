# Boost.Spirit example

Parsing and generating SNOMED expressions directly from the compositional grammar (SCG).

Note: this example intentionally uses a limited subset of the SCG for
expositional purposes. It is possible to extend the embedded grammar
and generator to support the full syntax.

## Use Notes

Build the `scg` program using CMake. The program reads SCG expressions
from standard input. These expressions are parsed using Spirit.Qi into an
internal AST representation and, on success, echoed back to standard
output using Spirit.Karma.

See `cg-examples.txt` for examples of the SCG subset which the program
handles, and also details of the input file format.

`
$ scg < cg-examples.txt
`
