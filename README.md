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
N/A     | IP            | Instruction pointer (not accessible by program)
0       | BP            | Stack base pointer
1 .. 31 | R1 .. R31     | General Purpose


##### Instructions

Code    | Instruction   | Arguments                 | Action
---     | ---           | ---                       | ---
0x00    | nop           | N/A                       | Nothing
0x01    | halt          | N/A                       | Halts the program
0x??    | mov           | [reg1], [reg2]            | Copies the value of [reg1] to [reg2]
0x??    | movl          | [reg1], [lit]             | Puts the literal value [lit] in [reg1]
0x??    | load          | [reg1], [reg2], [off]     | Load into [reg1] the value at address [reg2] + [off]
0x??    | store         | [reg1], [off],            | Store the value from [reg2] at address [reg1] + [off]
0x??    | jmp           | [off]                     | Relative jump by [off]
0x??    | fjmp          | [addr]                    | Fixed jump to [addr]
0x??    | iadd          | [reg1], [reg2], [reg3]    | Adds the values from [reg2] and [reg3] into [reg1] as integers


##### Stack

##### Examples
```
; Adds 16 and -2, stores the result on the stack
movl R0, 0x000000F0 (decimal 16)
movl R1, 0xFFFFFFFE (decimal -2)
iadd R0, R0, R1
store R0, BP, 0xFFFFFFFFC (decimal -4)
```