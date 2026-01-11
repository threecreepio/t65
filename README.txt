# Terminal 6502

Simple 6502 emulator for running in terminal.

It can read simple NROM NES ROMS or compile and run assembly files with asm6 by loopy (https://github.com/parasyte/asm6/tree/master)

## Usage

`t65 [--quiet] <inputfile>`

The quiet flag will disable the CPU tracelog output.

An input file ending in .s or .asm is assumed to be an assembly file. Anything else will be loaded as an NES NROM.

## Special handling

Whenever a BRK or NOP instruction is executed, execution will halt until an input is pressed.

If an RTS is executed from the top of the stack the program will exit with the A register as its exit code.

There are custom registers to perform useful things, just one, really:

`$5000` - print an ASCII character to the terminal

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
make install

t65 examples/helloworld.s -q
```


