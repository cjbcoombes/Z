# Z
An interpreted programming language with RISC bytecode, and c-style syntax, written in c++

## Running It
Compiles (for me) in Microsoft Visual Studio Community 2019 Version 16.10.2

Currently my command line arguments are:\
`z1_0.exe -debug -assemble "file.azm" "file.eze" -profile -exec "file.eze"`

Command       | Meaning
---           | ---    
debug         | Turns on debug mode. Only affects "assemble" and "exec" commands after this command.
nodebug       | Turns off debug mode. Only affects "assemble" and "exec" commands after this command.
profile       | Turns on profile mode. Only affects "exec" commands after this command.
assemble      | Assembles the first file argument (text, .azm) into the second file argument (binary, .eze)
exec          | Executes the first file argument (binary, .eze)

## VM
#### Bytecode
A register-based RISC bytecode

#### AZM File Format
Anything in parentheses or following a semicolon until a new line is a comment. Ex `(comment)`, `; comment...`
Everything else is instructions and their arguments separated by whitespace or commas. 
Anything in double-quotes is treated as a single object (whitespace and commas are not terminators/separators, some escape codes, such as `\"` for ", work).
See below section **Examples** for examples.

#### EZE File Format
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
2       | NR            | Null Register, does nothing
3 .. 32 | R0 .. R29     | General Purpose


##### Instructions

TODO: put in all of the number codes once finalized \

Possible Arguments: \
[reg] is a 1-byte register ID \
[off] is a 2-byte offset used for branching \
[byte] is a byte \
[word] is a 4-byte word. A [label] can go in the place of any word, and the program address will be inserted there\
[label] is a label, starting with "@", that will be converted to a program address for branching \
[var] is the name of a global, used for storing and recalling globals \
[string] is a null-terminated string

(F) means it sets the flags based on the result \
\{addr\} means the value at address addr

Code    | Instruction   | Arguments                 | Equation                       | Action
---     | ---           | ---                       | ---                            | ---
0x00    | nop           | N/A                       | N/A                            | Nothing
0x01    | halt          | N/A                       | N/A                            | Halts the program
0x??    | strprnt       | [reg1]                    | N/A                            | Prints the null-terminated string starting at the address in [reg1]
0x??    | rprnt         | [reg1]                    | N/A                            | Prints a hex representation of [reg1]
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
0x??    | icmp      (F) | [reg1]                    | N/A                            | Sets the flags based on the integer value in [reg1]
0x??    | icmpeq    (F) | [reg1], [reg2], [reg3]    | [reg1] = [reg2] == [reg3]      | Checks if the integers in [reg2] and [reg3] are equal, stores the boolean result in [reg1]
0x??    | icmpne    (F) | [reg1], [reg2], [reg3]    | [reg1] = [reg2] != [reg3]      | Checks if the integers in [reg2] and [reg3] are not equal, stores the boolean result in [reg1]
0x??    | icmpgt    (F) | [reg1], [reg2], [reg3]    | [reg1] = [reg2] > [reg3]       | Checks if the integer in [reg2] is greater than that in [reg3], stores the boolean result in [reg1]
0x??    | icmplt    (F) | [reg1], [reg2], [reg3]    | [reg1] = [reg2] < [reg3]       | Checks if the integer in [reg2] is less than that in [reg3], stores the boolean result in [reg1]
0x??    | icmpge    (F) | [reg1], [reg2], [reg3]    | [reg1] = [reg2] >= [reg3]      | Checks if the integer in [reg2] is greater than or equal to that in [reg3], stores the boolean result in [reg1]
0x??    | icmple    (F) | [reg1], [reg2], [reg3]    | [reg1] = [reg2] <= [reg3]      | Checks if the integer in [reg2] is less than or equal to that in [reg3], stores the boolean result in [reg1]
0x??    | iinc      (F) | [reg1]                    | [reg1] = [reg1] + 1            | Increments the integer value in [reg1] by 1
0x??    | idec      (F) | [reg1]                    | [reg1] = [reg1] - 1            | Decrements the integer value in [reg1] by 1
0x??    | iadd      (F) | [reg1], [reg2], [reg3]    | [reg1] = [reg2] + [reg3]       | Adds the values from [reg2] and [reg3] into [reg1] as integers
0x??    | isub      (F) | [reg1], [reg2], [reg3]    | [reg1] = [reg2] + [reg3]       | Subtracts the value in [reg3] from the value in [reg2] into [reg1] as integers
0x??    | imul      (F) | [reg1], [reg2], [reg3]    | [reg1] = [reg2] + [reg3]       | Multiplies the values from [reg2] and [reg3] into [reg1] as integers
0x??    | idiv      (F) | [reg1], [reg2], [reg3]    | [reg1] = [reg2] + [reg3]       | Divides the value in [reg2] by the value in [reg3] into [reg1] as integers. Throws divide by zero error if the value in [reg3] is zero.
0x??    | imod      (F) | [reg1], [reg2], [reg3]    | [reg1] = [reg2] + [reg3]       | Puts value from [reg2] modulo the value in [reg3] into [reg1] as integers. Throws divide by zero error if the value in [reg3] is zero.
N/A     | N/A           | N/A                       | N/A                            | Separates valid from invalid opcodes. The remaining opcodes are available as commands in assembly, but are converted to the opcodes above in order to be executed
N/A     | loadgw        | [reg1], [var]             | [reg1] = [var]                 | Load the word at [var] into [reg1]
N/A     | storegw       | [var], [reg1]             | [var] = [reg1]                 | Store the word in [reg1] at [var]
N/A     | loadgb        | [reg1], [var]             | [reg1] = [var]                 | Load the byte at [var] into [reg1]
N/A     | storegb       | [var], [reg1]             | [var] = [reg1]                 | Store the byte in [reg1] at [var]
N/A     | strprntg      | [var]                     | N/A                            | Print the null-terminated string starting at [var]
N/A     | N/A           | N/A                       | N/A                            | Separates global and non-global opcodes. The below opcodes must come before all others in a program, and are used to define globals
N/A     | globalw       | [var], [word]             | [var] = [word]                 | Sets global [var] to [word]
N/A     | globalb       | [var], [byte]             | [var] = [byte]                 | Sets global [var] to [byte]
N/A     | globalstr     | [var], [string]           | [var] = [string]               | Sets global [var] to [string]

##### Stack

##### Examples
Note: .azm files should be up-to-date with the bytecode, but .eze files might require regeneration. \
Another Note: Many of these add a register to 0xFFFFFFFF (decimal -1) to do subtraction. That's because the increment and decrement instructions didn't exist at the time.
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
```
; Files: example5.azm, example5.eze
; Tests various mathematical operations
movw R0, 0x00000019 ; # of iterations
movw R1, 0xFFFFFFFF ; -1
movw R2, 0x00000011 ; Starting #
movw R3, 0x00000002 ; # = # - R3
                    ; # = # * R3
movw R4, 0x00000100 ; # = # % R4
@JUMP
rprnt R2
lnprnt
isub R2, R2, R3
imul R2, R2, R3
imod R2, R2, R4
iadd R0, R0, R1
jmpnz @JUMP
```
```
; Files: example6.azm, example6.eze
; Tests division by zero (should throw error)
movw R1, 0xFFFFFFFF
movw R2, 0x01000000 
movw R3, 0x00000100
@JUMP
idiv R4, R2, R3
iadd R3, R3, R1
rprnt R4
lnprnt
jmp @JUMP
```
```
; Files: example6.azm, example6.eze
; Tests some comparison operators
globalstr GTR, "Greater"
globalstr LSS, "Less"
globalstr EQ, "Equal"

movw R1, 0xFFFFFFFF (-1)
movw R2, 0x00000020 (32) ; Counter
movw R3, 0x00000010 (16) ; Constant

@ITER
lnprnt
iadd R2, R2, R1   ; R2--
icmp R2           ; If counter is zero:
jmpz @END         ; halt
icmpgt NR, R2, R3 ; If R2 > R3:
jmpnz @GTR        ; Jump to @GTR
icmplt NR, R2, R3 ; If R2 < R3:
jmpnz @LSS        ; Jump to @LSS
jmp @EQ           ; Else (R2 == R3), jump to @EQ

@GTR
strprntg GTR
jmp @ITER

@LSS
strprntg LSS
jmp @ITER

@EQ
strprntg EQ
jmp @ITER

@END
```
```
; Files: fibonacci2.azm, fibonacci2.eze
; Fibonacci using recursion and the stack
; Basically: FIB(x) = 1 if x <= 2
;                       else FIB(x - 1) + FIB(x - 2)
globalw N, 0x10

@FIBONACCI
; Stack
;             | BP points here  | BP + 4          | BP + 8
; Other stuff | BP to return to | IP to return to | Argument

; Return value goes in R0

movw R1, 0x00000002     ; Decimal 2
loadw R2, BP, 0x8       ; Load argument into register
icmple NR, R2, R1       ; Compare (dump boolean output)
jmpz @CALC              ; If > 2, jump to CALC
movw R0, 0x00000001     ; Else, set return value to 0
jmp @RETURN             ; Jump to RETURN

@CALC
idec R2                 ; Decrement argument
storew BP, 0xC, R2      ; Store for later
storew BP, 0x10, BP     ; Put BP on end of stack
movw R3, @CALC_1        ; Put return IP in register
storew BP, 0x14, R3     ; And then on the stack
storew BP, 0x18, R2     ; Put argument on the end of the stack
movw R1, 0x00000010     ; Decimal 16
iadd BP, BP, R1         ; Set new BP
jmp @FIBONACCI
@CALC_1
loadw BP, BP, 0x0       ; Reset BP

loadw R2, BP, 0xC       ; Recover argument
storew BP, 0xC, R0      ; Store result for later

; Do it again
idec R2                 ; Decrement argument
storew BP, 0x10, BP     ; Put BP on end of stack
movw R3, @CALC_2        ; Put return IP in register
storew BP, 0x14, R3     ; And then on the stack
storew BP, 0x18, R2     ; Put argument on the end of the stack
movw R1, 0x00000010     ; Decimal 16
iadd BP, BP, R1         ; Set new BP
jmp @FIBONACCI
@CALC_2
loadw BP, BP, 0x0       ; Reset BP

loadw R2, BP, 0xC       ; Recover previous result
iadd R0, R0, R2         ; Add!
; Auto continues to RETURN

@RETURN
loadw R3, BP, 0x4       ; Load IP to return to
jmpr R3                 ; Jump to IP (caller is responsible for handling BP)


@__START__
storew BP, 0x0, BP      ; Put BP on end of stack
movw R3, @END           ; Put return IP in register
storew BP, 0x4, R3      ; And then on the stack
loadgw R2, N            ; Starting argument
storew BP, 0x8, R2      ; Put argument on the end of the stack
jmp @FIBONACCI
@END
rprnt R0
lnprnt
halt
```