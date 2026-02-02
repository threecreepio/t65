@Continue:
lda HelloWorldASCII,y  ; fetch each ascii character
beq @Done              ; loop until null terminator found
sta $5000              ; print character
sta $5001              ; print character
iny                    ; and advance
bne @Continue          ;
@Done:
rts                    ; rts to exit, exit code is the A register, 0 in this case
HelloWorldASCII:
.byte "Hello, World.",$0A,$00
