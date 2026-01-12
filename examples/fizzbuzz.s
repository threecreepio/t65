PRINT         = $5000   ; print to stdout
CURRENT       = $1      ; tracks current iteration
MODULO_TMP    = $2      ; temp value for modulo calculation
MODCHECK      = $3      ; divisibility flag

@MainLoop:
    inc CURRENT         ; advance to next value to check, starting at 1

    lda #0              ; clear divisibility check
    sta MODCHECK        ;

    lda CURRENT         ; load current value
    jsr DivideBy3       ; check if evenly divisible by 3
    bne @Check5         ; if not - skip ahead
    inc MODCHECK        ; otherwise we flag divisibility
    jsr PrintFizz       ; and print our fizz value

@Check5:
    lda CURRENT         ; load current value
    jsr DivideBy5       ; and check if evenly divisible by 5
    bne @PrintValue     ; if not - skip ahead
    inc MODCHECK        ; otherwise we flag divisibility
    jsr PrintBuzz       ; aaand print our buzz value

@PrintValue:
    lda MODCHECK        ; check if we were evenly divisible by 3 or 5
    bne @Advance        ; if we were - skip ahead
    lda CURRENT         ; otherwise reload current value
    jsr PrintBCD        ; and print it as a decimal value

@Advance:
    lda #$0A            ; print a newline
    sta PRINT           ;
    lda CURRENT         ; check if we're reached the final value
    cmp #100          ;
    bne @MainLoop       ; if not we loop back

@Done:
    rts                 ; otherwise end program execution

DivideBy3:
    ldy #3              ; select divisor
    jmp DivideByY       ;

DivideBy5:
    ldy #5              ; select divisor

DivideByY:
    sty MODULO_TMP      ; store value temporarily
    ldx #0              ; clear X register for a division counter
    sec                 ;
@Modulus:
    inx                 ; advance counter
    sbc MODULO_TMP      ; and subtract our value from A
    bcs @Modulus        ; loop until we go negative (so, only works up to 128, fine for this)
    dex                 ; decrement X since we overrun by 1
    adc MODULO_TMP      ; and add our divisor back on, for the same reason
    rts                 ; and we're done!

PrintFizz:
    ldy #0              ;
@Next:
    lda Fizz,y          ; step through each value of our text
    beq @Done           ; exit at null terminator
    sta PRINT           ; otherwise print the next character
    iny                 ; and advance!
    bne @Next           ; keep looping until null terminator found
@Done:
    rts                 ; done!

Fizz: .byte "FIZZ",$00

PrintBuzz:
    ldy #0              ;
@Next:
    lda Buzz,y          ; step through each value of our text
    beq @Done           ; exit at null terminator
    sta PRINT           ; otherwise print the next character
    iny                 ; and advance!
    bne @Next           ; keep looping until null terminator found
@Done:
    rts                 ; done!

Buzz: .byte "BUZZ",$00

PrintBCD:
    ldy #10             ; start by dividing our value by 10
    jsr DivideByY       ;
    cpx #0              ; check if we have a tens digit
    beq @PrintOnes      ; otherwise just print ones
    pha                 ; printing 10s, start by storing the ones on stack
    txa                 ; transfer the tens to accumulator
    clc                 ;
    adc #'0'            ; shift up to ASCII 0
    sta PRINT           ; and print!
    pla                 ; then restore our ones digit
@PrintOnes:
    clc                 ;
    adc #'0'            ; shift up to ASCII 0
    sta PRINT           ; and print!
    rts                 ; done!
