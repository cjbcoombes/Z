# Z
An interpreted programming language with RISC bytecode, and c-style syntax, written in c++

## VM
#### Bytecode
A register-based RISC bytecode

##### Registers
The bytecode has several general-purpose registers that can hold any 32-bit value (including memory addresses).
There are also several special-purpose registers

ID      | Register      | Purpose
---     | ---           | ---
N/A     | N/A           | Instruction pointer (not accessible by program)
0       | BP            | Stack base pointer
1 .. 31 | R0 .. R31     | General Purpose


##### Instructions

Code    | Instruction   | Arguments     | Action
---     | ---           | ---           | ---
0x00    | NOP           | N/A           | Nothing
0x01    | HALT          | N/A           | Halts the program

##### Stack

##### Examples
```
; Adds 16 and -2, stores the result on the stack
movl R0, 0x000000F0 (decimal 16)
movl R1, 0xFFFFFFFE (decimal -2)
add R0, R0, R1
store R0, BP, 0xFFFFFFFFB (decimal -4)
```