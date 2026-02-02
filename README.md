# Terminal 6502

Simple 6502 emulator for running in terminal.

Compiles and runs assembly files with asm6 by loopy (https://github.com/parasyte/asm6/tree/master)

## Usage

`t65 [--quiet] <inputfile>`

The quiet flag will disable the CPU tracelog output.

The input should be a single assembly file, which will be loaded at $8000.

## Tracelog format

```
$8000> LDA $800C,Y         A:00 X:00 Y:00 S:FD P:I       CYC:0
$8003> BEQ $800B           A:48 X:00 Y:00 S:FD P:I       CYC:4
$8005> STA $5000           A:48 X:00 Y:00 S:FD P:I       CYC:6
H
```

Prints out the register state, number of executed cycles, and the next instruction that will run.

## Special handling

When tracing is on, a BRK instruction will halt execution until an input is pressed. Otherwise BRK exits the program.

If an RTS is executed from the top of the stack the program will exit with the A register as its exit code.

There are custom registers to perform useful things.

`$5000` - print an ASCII character to the terminal
`$5001` - switch tracing on (1) or off (0)
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
rts                    ; rts to exit, exit code is the A register, 0 in this case
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


