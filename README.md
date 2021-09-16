For information on the bytecode, see the README in the appropriate project folder. The most up-to-date one (which is not very) is in `Z (Attempt 2)`

# The Language

One of the core concepts of **Z** is the way it handles types. Functions and sets, as well as primitives are first-class, and they each have a well-defined type. Some types are "compatible," meaning that they can be implicitly converted. The other key idea is a "named type," where the name can be used for variable names, parameter names, or accessor names depending on the context.
```c
// Type names are in angle brackets
<int>
// Brackets can be left out when the type does not contain any symbols
int

// To define a named type, use the "." operator after a type
// These two named types with no other context will define variables
int.a = 3
<int>.b = 2
// This is a valid named type, but it will not define a variable because the name is inside the brackets
<int.c>

// This is a set of an int, a float, and a char
<int, float, char>.foo = [7, -2.93, 'q']
foo[0] // Accesses the first element of the set, which is the integer 7

// This has the same types as foo, thus is compatible and implicitly converted
<int.iVal, float.fVal, char.cVal>.bar = foo;
bar[0]   // Accesses the same value as foo[0] did
bar.iVal // Accesses the same value via it's name

// The following have the same effect. One initializes in order, the other using the names of types
bar = [-4, 17.2, '*']
bar = [fVal: 17.2, cVal: '*', iVal: -4]

// The @ symbol means to implicitly assume the type
// In this case <int.x, int.y: float>, the type of a function taking two integer values (named x and y), and returning a float
@.func = <int.x, int.y: float>.{
    x += y;
    y--;
    return x + y;// Implicit conversion to float
}

// Compatible type, different parameter names
<int.a, int.b: float>.alt = func

// Function calls consume a set with a type that matches the parameters. The set can be created using parameter names
func[10, 7]
func[y: 7, x: 10]

// Equivalent, with different parameter names
alt[10, 7]
alt[b: 7, a: 10]

// Functions can also be called with a variable as the set input
<int, int>.input = [10, 7]
// Technically valid (since function call consumes the next token)
func input
// But this looks better:
func(input)
```















