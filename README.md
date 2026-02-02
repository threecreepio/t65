# Terminal 6502

A simple 6502 emulator that runs in the terminal.

Compiles and runs assembly files with **asm6** by loopy (https://github.com/parasyte/asm6)

## Usage

`t65 [-q|--quiet] <inputfile>`

The quiet flag will disable the CPU tracelog output.

The input should be a single assembly file, which will be loaded at $8000.

## Tracelog format

```
$8045> 60       |       RTS                                 A:01 X:00 Y:0A S:F9 P:I..C.. CYC:   133 (+3) 
$8071> E0 00    |     CPX #$00                              A:01 X:00 Y:0A S:FB P:I..C.. CYC:   139 (+6) 
$8073> F0 09 48 |     BEQ $807E                             A:01 X:00 Y:0A S:FB P:IZ.C.. CYC:   141 (+2) 
$807E> 18       |     CLC                                   A:01 X:00 Y:0A S:FB P:IZ.C.. CYC:   144 (+3) 
$807F> 69 30    |     ADC #$30                              A:01 X:00 Y:0A S:FB P:IZ.... CYC:   146 (+2) 
$8081> 8D 00 50 |     STA $5000                             A:31 X:00 Y:0A S:FB P:I..... CYC:   148 (+2) 1
$8084> 60       |     RTS                                   A:31 X:00 Y:0A S:FB P:I..... CYC:   152 (+4) 
$8027> A9 0A    |   LDA #$0A                                A:31 X:00 Y:0A S:FD P:I..... CYC:   158 (+6) 
```

Layout:

```
<PC> <opcode bytes> | <indent><mnemonic operands> <registers> <flags> CYC:<cpu cycles> (+<delta>) [output]
```

Each trace line shows the next instruction to execute, its raw opcode bytes, call depth, register and flag state, total CPU cycle count, and the cycle cost of the previously executed instruction.

The instruction shown is the next instruction that will run; the register and flag state reflect the CPU state immediately before that instruction executes.

Trace output is written to stderr.

## Special handling

When tracing is on, a BRK instruction will halt execution until an input is pressed. Otherwise BRK exits the program.

If an RTS is executed from the top of the stack the program will exit with the A register as its exit code.

There are custom registers to perform useful things.

`$5000` - print an ASCII character to stdout
`$5001` - switch tracing on (1) or off (0) and resets the cycle counter to 0
`$5002` - enable tracing and trigger breakpoint
`$5003` - exit application with a return code

## Example source file

```
@Continue:
lda HelloWorldASCII,y  ; fetch each ascii character
beq @Done              ; loop until null terminator found
sta $5000              ; print character
iny                    ; and advance
bne @Continue          ;
@Done:
rts                    ; exit program, exit code is the A register, 0 in this case
HelloWorldASCII:
.byte "Hello, World.",$0A,$00
```

## Building

Should build in any normal posix-y environment.

```
git clone https://www.github.com/threecreepio/t65
cd t65
make
make install # you can just copy the t65 binary wherever, this copies to /usr/local/bin

./t65 examples/helloworld.s -q
```


