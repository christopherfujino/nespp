; PRG CRC32 checksum: 1361c337
; CHR CRC32 checksum: d8f49994
; Overall CRC32 checksum: 339ca253
; Code base address: $8000

.setcpu "6502x"
.segment "HEADER"

.byte "NES", $1a                 ; Magic string that always begins an iNES header
.byte $02                        ; Number of 16KB PRG-ROM banks
.byte $01                        ; Number of 8KB CHR-ROM banks
.byte $00                        ; Control bits 1
.byte $00                        ; Control bits 2
.byte $00                        ; Number of 8KB PRG-RAM banks
.byte $00                        ; Video format NTSC/PAL

APU_PL1_HI = $4003
APU_PL1_LO = $4002
APU_PL1_VOL = $4000
APU_SND_CHN = $4015


.segment "CODE"

Reset:
  lda #$01                       ; $8000  A9 01
  sta APU_SND_CHN                ; $8002  8D 15 40
  lda #$08                       ; $8005  A9 08
  sta APU_PL1_LO                 ; $8007  8D 02 40
  lda #$02                       ; $800A  A9 02
  sta APU_PL1_HI                 ; $800C  8D 03 40
  lda #$BF                       ; $800F  A9 BF
  sta APU_PL1_VOL                ; $8011  8D 00 40

_label_8014:
  jmp _label_8014                ; $8014  4C 14 80

.segment "TILES"


.segment "VECTORS"

.addr 0, Reset, 0
