.segment "HEADER"

.byte "NES", $1A    ; iNESheader
.byte $02                        ; Number of 16KB PRG-ROM banks
.byte $01                        ; Number of 8KB CHR-ROM banks

.segment "CODE"

Reset:
  sei          ; 78
  ldx #$FF
  txs          ; initialize SP

Start:
  ldx #$69
  jsr Sub

  jmp Start

Sub:
  inx
  rts

.segment "VECTORS"

.word $00 ; nmi
.word Reset
.word $00 ; irq
