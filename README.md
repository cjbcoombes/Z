# Z
An interpreted programming language with RISC bytecode, and c-style syntax, written in c++

## VM
#### Bytecode
A register-based RISC bytecode

#### File Format
First 4 bytes: Address of first instruction \
Next (?) bytes: global memory \
Next (?) bytes: program

##### Registers
The bytecode has several general-purpose registers that can hold any word value (including memory addresses).
There are also several special-purpose registers.

ID      | Register      | Purpose
---     | ---           | ---
N/A     | IP            | Instruction pointer (not accessible by program)
0       | BP            | Stack base pointer
1       | GP            | Pointer to base of globals memory (includes the 4-byte pointer to the program start)
2 .. 31 | R2 .. R31     | General Purpose


##### Instructions

TODO: put in all of the number codes once finalized \

Possible Arguments: \
[reg] is a 1-byte register ID \
[off] is a 2-byte offset used for branching \
[byte] is a byte \
[word] is a 4-byte word \
[label] is a label, starting with "@", that will be converted to a program address for branching \
[var] is the name of a global, used for storing and recalling globals \
[string] is a null-terminated string \

F means it sets the flags based on the result \

Code    | Instruction   | Arguments                 | Action
---     | ---           | ---                       | ---
0x00    | nop           | N/A                       | Nothing
0x01    | halt          | N/A                       | Halts the program
0x??    | strprnt       | [reg1]                    | Prints the null-terminated string starting at the address in [reg1]
0x??    | rprnt         | [reg1]                    | Prints a hex representation of [reg1]
0x??    | lnprnt        | N/A                       | Prints a newline character
0x??    | mov           | [reg1], [reg2]            | Copies [reg1] to [reg2]
0x??    | movw          | [reg1], [word]            | Puts the word value [word] into [reg1]
0x??    | movb          | [reg1], [byte]            | Puts the byte value [byte] into [reg1]
0x??    | loadw         | [reg1], [reg2], [off]     | Load the word at address [reg2] + [off] into [reg1] 
0x??    | storew        | [reg1], [off], [reg2]     | Store the word from [reg2] at address [reg1] + [off]
0x??    | loadb         | [reg1], [reg2], [off]     | Load the byte at address [reg2] + [off] into [reg1] 
0x??    | storeb        | [reg1], [off], [reg2]     | Store the byte from [reg2] at address [reg1] + [off]
0x??    | jmp           | [label]                   | Jump to [label]
0x??    | jmpz          | [label]                   | Jump to [label] if the zero flag is set
0x??    | jmpnz         | [label]                   | Jump to [label] if the zero flag is not set
0x??    | icmp        F | [reg1]                    | Sets the flags based on the integer value in [reg1]
0x??    | iadd        F | [reg1], [reg2], [reg3]    | Adds the values from [reg2] and [reg3] into [reg1] as integers
N/A     | N/A           | N/A                       | Separates valid from invalid opcodes. The remaining opcodes are available as commands in assembly, but are converted to the opcodes above in order to be executed
N/A     | loadgw        | [reg1], [var]             | Load the word at [var] into [reg1]
N/A     | storegw       | [var], [reg1]             | Store the word in [reg1] at [var]
N/A     | loadgb        | [reg1], [var]             | Load the byte at [var] into [reg1]
N/A     | storegb       | [var], [reg1]             | Store the byte in [reg1] at [var]
N/A     | strprntg      | [var]                     | Print the null-terminated string starting at [var]
N/A     | N/A           | N/A                       | Separates global and non-global opcodes. The below opcodes must come before all others in a program, and are used to define globals
N/A     | globalw       | [var], [word]             | Sets global [var] to [word]
N/A     | globalb       | [var], [byte]             | Sets global [var] to [byte]
N/A     | globalstr     | [var], [string]           | Sets global [var] to [string]

##### Stack

##### Examples
```
; Add 16 and -2 as integers
movw R0, 0x000000F0 (decimal 16)
movw R1, 0xFFFFFFFE (decimal -2)
iadd R0, R0, R1
rprnt R0
```
```
; Files: example1.azm, example1.eze
; Add 1 and 2 as integers, with some branching for fun
movw R0, 0x00000001
jmp @SKIP
@SKIPBACK
iadd R2, R0, R1
jmp @END
@SKIP
movw R1, 0x00000002
jmp @SKIPBACK
@END
rprnt R2
```
```
; Files: example3.azm, example3.eze
; Read from a global variable, do some addition
globalw varname, 0x12345678
movw R0, 0x1
rprnt R0
movw R0, 0x2
loadgw R1, varname
iadd R0, R0, R1
rprnt R0
```
```
; Files: example4.azm, example4.eze
; Store a global string, print it
globalw WORD, 0x12ABCDEF
globalstr STR, "ABCD...\"Hello, my name is Chris\"\\"
loadgw R0, WORD
rprnt R0
lnprnt
loadgw R1, STR
rprnt R1
lnprnt
strprntg STR
lnprnt
```
```
; Files: fibonacci1.azm, fibonacci1.eze
; Prints the first 25 (0x19) fibonacci numbers
movw R0, 0x1
movw R1, 0x0
movw R2, 0x00000019
movw R3, 0xFFFFFFFF
@LOOP
iadd R1, R0, R1
iadd R2, R2, R3
rprnt R1
lnprnt
jmpz @END
iadd R0, R0, R1
iadd R2, R2, R3
rprnt R0
lnprnt
jmpnz @LOOP
@END
```
