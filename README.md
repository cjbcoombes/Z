# Z
An interpreted programming language with RISC bytecode, and c-style syntax, written in c++

## VM
#### Bytecode
A register-based RISC bytecode

#### File Format
First 4 bytes: Size of global table \
Second 4 bytes: Address of first instruction \
Next (?) bytes: global table \
Next (?) bytes: program

##### Registers
The bytecode has several general-purpose registers that can hold any word value (including memory addresses).
There are also several special-purpose registers

ID      | Register      | Purpose
---     | ---           | ---
N/A     | IP            | Instruction pointer (not accessible by program)
0       | BP            | Stack base pointer
1       | GP            | Pointer to base of globals memory (including constants, such as constant strings)
2 .. 31 | R2 .. R31     | General Purpose


##### Instructions

Code    | Instruction   | Arguments                 | Action
---     | ---           | ---                       | ---
0x00    | nop           | N/A                       | Nothing
0x01    | halt          | N/A                       | Halts the program
0x??    | strprnt       | [reg1]                    | Prints the null-terminated string starting at address [reg1]
0x??    | rprnt         | [reg1]                    | Prints a hex representation of [reg1]
0x??    | lnprnt        | N/A                       | Prints a newline character
0x??    | mov           | [reg1], [reg2]            | Copies [reg1] to [reg2]
0x??    | movw          | [reg1], [word]            | Puts the word value [word] in [reg1]
0x??    | movb          | [reg1], [byte]            | Puts the byte value [byte] in [reg1]
0x??    | loadw         | [reg1], [reg2], [off]     | Load the word at address [reg2] + [off] into [reg1] 
0x??    | storew        | [reg1], [off], [reg2]     | Store the word from [reg2] at address [reg1] + [off]
0x??    | loadb         | [reg1], [reg2], [off]     | Load the byte at address [reg2] + [off] into [reg1] 
0x??    | storeb        | [reg1], [off], [reg2]     | Store the byte from [reg2] at address [reg1] + [off]
0x??    | iadd          | [reg1], [reg2], [reg3]    | Adds the values from [reg2] and [reg3] into [reg1] as integers


##### Stack

##### Examples
```
; Add 16 and -2 as integers
movw R0, 0x000000F0 (decimal 16)
movw R1, 0xFFFFFFFE (decimal -2)
iadd R0, R0, R1
rprnt R0
```