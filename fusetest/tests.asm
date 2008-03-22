; Tests run by the test harness

; Copyright 2007-2008 Philip Kendall <philip-fuse@shadowmagic.org.uk>
	
; This program is licensed under the GNU General Public License. See the
; file `COPYING' for details
	
; Check undocumented flags after BIT n,(IX+d) (bug #1726543)
	
bitnixtest
PROC
	ld ix, _data - 0x44
	bit 5, (ix+0x44)
	push af
	pop bc
	ld a, 0x30
	cp c
	ret z
	ld b, 0x00
	ld a,c
	ret

_data	defb 0xff
	
ENDP
	
; Check for the undocumented behaviour of DAA (patch #1724193)
	
daatest
PROC
	ld bc, 0x9a02
	ld hl, 0xcbdd
	push bc
	pop af
	daa
	push af
	pop bc
	add hl, bc
	ld a, h
	or l
	ret z
	ld a, b
	ld b, 0x00
	ret
ENDP

; Check the behaviour of LDIR at contended memory boundary (revision 2841).
	
ldirtest
PROC
	ld bc, _delay1
	ld hl, _table1
	call guessmachine_table

	ld bc, _delay2
	ld hl, _table2
	call guessmachine_table
	
	call interruptsync

	cp 0x00			; 92
	jr nz, _fail		; 99

	ld hl, sync_isr + 1	; 106
	ld (hl), _isr % 0x100	; 116
	inc hl			; 126
	ld (hl), _isr / 0x100	; 132

	ld hl, (first_delay_1)	; 142
	call delay		; 158

	ld hl, (_delay1)	; 398
	call delay		; 414

				; 48K / 128K / +3 early timings
	ld hl, 0x0000		; 14289 / 14315 / 14322
	ld de, 0x7fff		; 14299 / 14325 / 14332
	ld bc, 0x0002		; 14309 / 14335 / 14342
	ldir			; 14319, 14358 / 14345, 14384 / 14352, 14380

	ld hl, (_delay2)	; 14374 / 14400 / 14400
	call delay		; 14390 / 14416 / 14416

	ld hl, (first_delay_2)	; 69099 / FIXME / FIXME
	call delay		; 69115 / FIXME / FIXME

	jp atiming		; 69355 / 70375 / 70375

_isr	pop hl
	ret

_fail	ld b, 0x02
	ret

_table1	defw 0x3633, 0x3633 + 0x001a, 0x3654, 0x3633
_table2	defw 0xd5b5, 0xd5b5 - 0x001a + 0x03fc, 0xd99b, 0xd5b5 + 0x0700 + 0x0012

_delay1	defw 0x0000
_delay2	defw 0x0000

ENDP

; Check for contended IN timings (part of bug #1708957)

contendedintest
PROC
	ld bc, contendedin_delay2
	ld hl, contendedin_table2
	call guessmachine_table

	ld bc, 0x40ff
	push bc

contendedin1
	ld bc, contendedin_delay1
	ld hl, _table1
	call guessmachine_table
	
	call interruptsync

	cp 0x00			; 92
	jr nz, _fail		; 99

	ld hl, sync_isr + 1	; 106
	ld (hl), _isr % 0x100	; 116
	inc hl			; 126
	ld (hl), _isr / 0x100	; 132

	ld hl, (first_delay_1)	; 142
	call delay		; 158

	ld hl, (contendedin_delay1) ; 398
	call delay		; 414

				; 48K / 128K / +3 timings
	pop bc			; 43036 / 43573 / 43574
	in a,(c)		; 43046 / 43584 / 43584

	ld hl, (contendedin_delay2) ; 43070 / 43608 / 43596
	call delay		; 43086 / 43624 / 43608

	ld hl, (first_delay_2)	; 69099 / FIXME / FIXME
	call delay		; 69115 / FIXME / FIXME

	jp atiming		; 69355 / 70375 / 70735

_isr	ld b, 0x00
	pop hl
	ret

_fail	ld b, 0x02
	ret

_table1	defw 0xa67e
	defw 0xa67e + 0x001a + 4 * 0x0080
	defw 0xa67e + 0x001a + 4 * 0x0080
	defw 0xa67e
contendedin_table2 defw 0x659d
	defw 0x659d - 0x001a - 4 * 0x0080 + 0x03fc
	defw 0x659d - 0x001a - 4 * 0x0080 + 0x03fc + 0x000c
	defw 0x659d + 0x0700 + 0x000c

contendedin_delay1 defw 0x0000
contendedin_delay2 defw 0x0000

ENDP

; Check for floating bus behaviour (rest of bug #1708957)

floatingbustest
PROC
	; No point running this test on the +3 or Pentagon
	ld a, (guessmachine_guess)
	cp 0x02
	jr nc, _skip

	ld bc, _delay
	ld hl, _table
	call guessmachine_table
	
	ld hl, 0x5a0f
	ld b,(hl)
	push bc
	ld (hl), 0x53
	
	call interruptsync

	cp 0x00			; 92
	jr nz, _fail		; 99

	ld hl, (first_delay_1)	; 106
	call delay		; 122

	ld hl, (_delay)		; 362
	call delay		; 378

				; 48K / 128K timings
	ld bc, 0x40ff		; 43019 / 43557
	ld d, 0x53		; 43029 / 43567

	ld hl, 0x5a0f		; 43036 / 43574

	in a,(c)		; 43046 / 43584
				; floating bus read at 43069 / 43607

	pop bc
	ld (hl),b

	cp d
	ld b, 0x00
	ret

_fail	pop bc
	ld hl, 0x5a0f
	ld (hl), b
	ld b, 0x02
	ret

_skip	ld b, 0x01
	add a, b
	ret

_table	defw 0xa691, 0xa691 + 0x001a + 4 * 0x0080
_delay	defw 0x0000

ENDP

; Test memory contention

contendedmemorytest
PROC
	ld bc, _delay1
	ld hl, _table1
	call guessmachine_table

	ld bc, _delay2
	ld hl, _table2
	call guessmachine_table
	
	ld hl, _nop
	ld de, 0x8001 - ( _nopend - _nop )
	ld bc, _nopend - _nop
	ldir
	
	call interruptsync
	
	cp 0x00			; 92
	jr nz, _fail		; 99

	ld hl, sync_isr + 1	; 106
	ld (hl), _isr % 0x100	; 116
	inc hl			; 126
	ld (hl), _isr / 0x100	; 132

	ld hl, (first_delay_1)	; 142
	call delay		; 158

	ld hl, (_delay1)	; 398
	call delay		; 414

				; 48K / 128K / +3 timings
	call 0x7fff		; 14318 / 14344 / 14346

	ld hl, (_delay2)	; 14355 / 14381 / 14384
	call delay		; 14371 / 14397 / 14400

	ld hl, (first_delay_2)	; 69099 / FIXME / FIXME
	call delay		; 69115 / FIXME / FIXME
	
	jp atiming		; 69355 / 70375 / 70375

_isr	pop hl
	ret

_fail	ld b, 0x02
	ret
	
_nop	nop			; 14335 / 14361 / 14363
	ret			; 14345 / 14371 / 14374
_nopend

_table1	defw 0x3650
	defw 0x3650 + 0x001a
	defw 0x3650 + 0x001a + 0x0002
	defw 0x3650
_table2	defw 0xd5c8
	defw 0xd5c8 - 0x001a + 0x03fc
	defw 0xd5c8 - 0x001a - 0x0003 + 0x03fc
	defw 0xd5c8 + 0x0700 + 0x0006

_delay1	defw 0x0000
_delay2	defw 0x0000

ENDP

; Test high port contention (part 1)

highporttest1
PROC
	ld bc, contendedin_delay2
	ld hl, _table
	call guessmachine_table

	ld a, (0x5b5c)
	and 0xf8
	ld bc, 0x7ffd
	out (c), a

	ld bc, 0xffff
	push bc

	jp contendedin1

_table	defw 0x65a9
	defw 0x65a9 - 0x001a - 4 * 0x0080 + 0x03fc
	defw 0x65a9 - 0x001a - 4 * 0x0080 + 0x03fc
	defw 0x65a9 + 0x0700

ENDP

; High port contention part 2

highporttest2
PROC
	ld a, (guessmachine_guess)
	cp 0x01
	jr c, _skip
	
	ld a, (0x5b5c)
	and 0xf8
	or 7
	ld bc, 0x7ffd
	out (c), a

	ld bc, contendedin_delay2
	ld hl, _table
	call guessmachine_table

	ld bc, 0xffff
	push bc

	jp contendedin1

_skip	ld b, 0x01
	ret

_table	defw 0x65a9		; Not used
	defw 0x65a9 - 0x001a - 4 * 0x0080 + 0x03fc - 0x000c
	defw 0x65a9 - 0x001a - 4 * 0x0080 + 0x03fc
	defw 0x65a9 + 0x0700

ENDP

; 0x7ffd read tests

hex3ffdreadtest
PROC
	ld hl, 0x3ffd
	push hl
	ld hl, _table
	push hl

	jp hex7ffdreadtest_common

_table	defw 0x0000
	defw 0x3696

ENDP

hex7ffdreadtest
PROC
	ld hl, 0x7ffd
	push hl
	ld hl, _table
	push hl

	jp hex7ffdreadtest_common
	
_table	defw 0x0000
	defw 0x368c

ENDP

hex7ffdreadtest_common
PROC
	ld a, (guessmachine_guess)
	cp 0x01
	jp nz, _skip

	ld bc, _delay
	pop hl
	call guessmachine_table

	ld bc, 0x0002
	ld de, _scratch
	ld hl, 0x4002
	ldir

	ld bc, 0x0002
	ld hl, 0x5802
	ldir

	ld bc, 0x0002
	ld de, 0x4002
	ld hl, _data
	ldir

	ld bc, 0x0002
	ld de, 0x5802
	ldir

	ld a, (0x5b5c)
	and 0xf8
	ld bc, 0x7ffd
	out (c), a

	call interruptsync

	cp 0x00
	jr nz, _fail

	ld hl, (first_delay_1)
	call delay

	ld hl, (_delay)
	call delay

	pop bc
	in a,(c)

	ld a, (0xe000)
	cp 0x00

	ld b, 0x00

_end	push af
	push bc

	ld a, (0x5b5c)
	ld bc, 0x7ffd
	out (c), a

	ld bc, 0x0002
	ld hl, _scratch
	ld de, 0x4002
	ldir

	ld bc, 0x0002
	ld de, 0x5802
	ldir
	
	pop bc
	pop af
	ret

_fail	pop hl
	ld b, 0x02
	jr _end

_skip	pop hl
	pop hl
	ld b, 0x01
	ret

_delay	defw 0x0000

_scratch defw 0x0000, 0x0000

_data	defb 0x01, 0x03, 0x02, 0x04

ENDP
