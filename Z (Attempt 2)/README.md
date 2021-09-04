# Z v2.0
An interpreted programming language with RISC bytecode, and c-style syntax, written in c++

## Changes from v1.0
The main reason for starting a new version was to clean up some of the code for the previous one. v1.0 has a working vm,
but that vm was created as I was learning how to design it, and as such has some flaws. The new version intends to take
that knowledge to streamline the code.

#### Major Changes:
 - 

## Running It
Compiles (for me) in Microsoft Visual Studio Community 2019 Version 16.10.2

Currently my command line arguments are:\
`z2_0.exe -debug -profile -asmandexec "file.azm" "file.eze"`

Command       | Meaning
---           | ---    
debug         | Turns on debug mode. Only affects "assemble," "exec," and "asmandexec" commands after this command.
nodebug       | Turns off debug mode. Only affects "assemble," "exec," and "asmandexec" commands after this command.
profile       | Turns on profile mode. Only affects "exec" and "asmandexec" commands after this command.
noprofile     | Turns off profile mode. Only affects "exec" and "asmandexec" commands after this command.
assemble      | Assembles the first file argument (text, .azm) into the second file argument (binary, .eze)
exec          | Executes the first file argument (binary, .eze)
asmandexec    | Assembles the first file argument (text, .azm) into the second file argument (binary, .eze), which is then executed
stacksize     | The first argument sets the stack size for execution. Only affects "exec" and "asmandexec" commands after this command.

## The Language
Nothin' here yet...

## VM
#### Bytecode
A register-based RISC bytecode

#### AZM File Format
Anything in parentheses or following a semicolon until a new line is a comment. Ex `(comment...)`, `; comment...`
Everything else is instructions and their arguments, with tokens separated by whitespace or commas. 
Anything in double-quotes is treated as a single object (whitespace and commas are not terminators/separators, some escape codes, such as `\"` for ", work).
Numbers use typical formatting: ### for decimal, 0x### for hex, 0b### for binary, -### for negative numbers. Be careful of overflow.
See below section **Examples** for examples.

#### EZE File Format
First 4 bytes: Address of first instruction \
Next (?) bytes: global memory \
Next (?) bytes: program

##### Registers
The bytecode has several general-purpose registers that can hold any word, byte, or short value (including memory addresses).
There are also several special-purpose registers.

ID          | Register      | Purpose
---         | ---           | ---
N/A         | IP            | Instruction pointer (not accessible by program)
0           | PP	    	| Program memory pointer (points to the base of the binary file loaded into memory)
1           | BP            | Stack base pointer
2           | FZ	    	| Zero flag register (zero flag is stored in the boolean part)
3 ... 17	| W0 .. 14	    | General purpose word
18 .. 31	| B0 .. 13  	| General purpose byte

##### Instructions

TODO: put in all of the number codes once finalized \

Possible Arguments: \
[reg] is a 1-byte register ID \
[off] is a 4-byte offset used for branching \
[byte] is a byte \
[word] is a 4-byte word. A [label] can go in the place of any word, and the program address will be inserted there \
[label] is a label, starting with "@", that will be converted to a program address for branching \
[var] is the address of a global, starting with "%", and can go in the place of any word \
[string] is a null-terminated string

(F) means it sets the flags based on the result \
\{addr\} means the value at address addr

Code    | Instruction   | Arguments                 | Equation                       | Action
---     | ---           | ---                       | ---                            | ---
0x00    | nop           | N/A                       | N/A                            | Nothing
0x01    | halt          | N/A                       | N/A                            | Halts the program
0x??    | strprnt       | [reg1]                    | N/A                            | Prints the null-terminated string starting at the address in [reg1]
0x??    | rprntw        | [reg1]                    | N/A                            | Prints the word value of [reg1]
0x??    | lnprnt        | N/A                       | N/A                            | Prints a newline character
0x??    | mov           | [reg1], [reg2]            | [reg1] = [reg2]                | Copies [reg2] to [reg1]
0x??    | movw          | [reg1], [word]            | [reg1] = [word]                | Puts the word value [word] into [reg1]
0x??    | movb          | [reg1], [byte]            | [reg1] = [byte]                | Puts the byte value [byte] into [reg1]
0x??    | loadw         | [reg1], [reg2], [off]     | [reg1] = {[reg2] + [off]}      | Load the word at address [reg2] + [off] into [reg1] 
0x??    | storew        | [reg1], [off], [reg2]     | {[reg1] + [off]} = [reg2]      | Store the word from [reg2] at address [reg1] + [off]
0x??    | loadb         | [reg1], [reg2], [off]     | [reg1] = {[reg2] + [off]}      | Load the byte at address [reg2] + [off] into [reg1] 
0x??    | storeb        | [reg1], [off], [reg2]     | {[reg1] + [off]} = [reg2]      | Store the byte from [reg2] at address [reg1] + [off]
0x??    | jmp           | [label]                   | N/A                            | Jump to [label]
0x??    | jmpz          | [label]                   | N/A                            | Jump to [label] if the zero flag is set
0x??    | jmpnz         | [label]                   | N/A                            | Jump to [label] if the zero flag is not set
0x??    | iflag     (F) | [reg1]                    | N/A                            | Sets the flags based on the integer value in [reg1]
0x??    | icmpeq    (F) | [reg1], [reg2]            | [reg2] == [reg3]               | Checks if the integers in [reg2] and [reg3] are equal, sets the flags according to the boolean result
0x??    | icmpne    (F) | [reg1], [reg2]            | [reg2] != [reg3]               | Checks if the integers in [reg2] and [reg3] are not equal, sets the flags according to the boolean result
0x??    | icmpgt    (F) | [reg1], [reg2]            | [reg2] > [reg3]                | Checks if the integer in [reg2] is greater than that in [reg3], sets the flags according to the boolean result
0x??    | icmplt    (F) | [reg1], [reg2]            | [reg2] < [reg3]                | Checks if the integer in [reg2] is less than that in [reg3], sets the flags according to the boolean result
0x??    | icmpge    (F) | [reg1], [reg2]            | [reg2] >= [reg3]               | Checks if the integer in [reg2] is greater than or equal to that in [reg3], sets the flags according to the boolean result
0x??    | icmple    (F) | [reg1], [reg2]            | [reg2] <= [reg3]               | Checks if the integer in [reg2] is less than or equal to that in [reg3], sets the flags according to the boolean result
0x??    | iinc      (F) | [reg1]                    | [reg1] = [reg1] + 1            | Increments the integer value in [reg1] by 1
0x??    | idec      (F) | [reg1]                    | [reg1] = [reg1] - 1            | Decrements the integer value in [reg1] by 1
0x??    | iadd      (F) | [reg1], [reg2], [reg3]    | [reg1] = [reg2] + [reg3]       | Adds the values from [reg2] and [reg3] into [reg1] as integers
0x??    | isub      (F) | [reg1], [reg2], [reg3]    | [reg1] = [reg2] + [reg3]       | Subtracts the value in [reg3] from the value in [reg2] into [reg1] as integers
0x??    | imul      (F) | [reg1], [reg2], [reg3]    | [reg1] = [reg2] + [reg3]       | Multiplies the values from [reg2] and [reg3] into [reg1] as integers
0x??    | idiv      (F) | [reg1], [reg2], [reg3]    | [reg1] = [reg2] + [reg3]       | Divides the value in [reg2] by the value in [reg3] into [reg1] as integers. Throws divide by zero error if the value in [reg3] is zero.
0x??    | imod      (F) | [reg1], [reg2], [reg3]    | [reg1] = [reg2] + [reg3]       | Puts value from [reg2] modulo the value in [reg3] into [reg1] as integers. Throws divide by zero error if the value in [reg3] is zero.
N/A     | N/A           | N/A                       | N/A                            | Separates global and non-global opcodes. The below opcodes must come before all others in a program, and are used to define globals
N/A     | globalw       | [var], [word]             | [var] = [word]                 | Sets global [var] to [word]
N/A     | globalb       | [var], [byte]             | [var] = [byte]                 | Sets global [var] to [byte]
N/A     | globalstr     | [var], [string]           | [var] = [string]               | Sets global [var] to [string]

##### Stack
The program provides a stack base pointer, BP, and that's it. See example `fibonacci_recursive.azm` for an example

##### Examples
Note: `.azm` files should be up-to-date with the bytecode, but `.eze` files might require regeneration. \
File path for examples: "Z\Z (Attempt 2)\AssemblyExamples\\"

**Recursive Fibonacci:** `fibonacci_recursive.azm / .eze`

**Fast Fibonacci:** `fibonacci_fast.azm / .eze`
