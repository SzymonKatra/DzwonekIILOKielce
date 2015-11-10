;---------------------------------------------------------------------------;
; MMC hardware controls and Flash controls      (C)ChaN, 2010
;---------------------------------------------------------------------------;
; Hardware dependent macros to be modified

;===========================================================================;
; Modifications by Szymon Katra
;===========================================================================;
; - removed flash control functions
; - reimplemented dly_100us in mmc.c due to errors

; MMC CS pin (DDR, PORT) - CS
#define	DDR_CS	_SFR_IO_ADDR(SDCARD_CS_DDR), SDCARD_CS_BIT
#define	PORT_CS	_SFR_IO_ADDR(SDCARD_CS_PORT), SDCARD_CS_BIT

; MMC SCLK pin (DDR, PORT) - SCK
#define	DDR_CK	_SFR_IO_ADDR(SDCARD_CK_DDR), SDCARD_CK_BIT
#define	PORT_CK	_SFR_IO_ADDR(SDCARD_CK_PORT), SDCARD_CK_BIT

; MMC DI pin (DDR, PORT) - MOSI
#define	DDR_DI	_SFR_IO_ADDR(SDCARD_DI_DDR), SDCARD_DI_BIT
#define	PORT_DI	_SFR_IO_ADDR(SDCARD_DI_PORT), SDCARD_DI_BIT

; MMC DO pin (DDR, PORT, PIN) - MISO
#define	DDR_DO	_SFR_IO_ADDR(SDCARD_DO_DDR), SDCARD_DO_BIT
#define	PORT_DO	_SFR_IO_ADDR(SDCARD_DO_PORT), SDCARD_DO_BIT
#define	PIN_DO	_SFR_IO_ADDR(SDCARD_DO_PIN), SDCARD_DO_BIT


;---------------------------------------------------------------------------;
.nolist
#include <avr/io.h>
#include "hardwareConfig.h"
.list
.text


;---------------------------------------------------------------------------;
; Initialize MMC port
;
; void init_spi (void);

.global init_spi
.func init_spi
init_spi:
	sbi	DDR_CS		; CS: output
	sbi	DDR_DI		; DI: output
	sbi	DDR_CK		; SCLK: output

	cbi DDR_DO      ; DO: input
	sbi	PORT_DO		; DO: pull-ups
	ret
.endfunc



;---------------------------------------------------------------------------;
; Delay 100 microseconds
;
; void dly_us (UINT n);

;.global dly_100us
;.func dly_100us
;dly_100us:
;	ldi	r24, lo8(F_CPU / 100000)	/* Loop counter */
;1:	sbiw	r30, 1		/* 10 clocks per loop */
;	sbiw	r30, 1
;	sbiw	r30, 1
;	nop
;	dec	r24
;	brne	1b
;	ret
;.endfunc



;---------------------------------------------------------------------------;
; Select MMC
;
; void select (void);

.global select
.func select
select:
	rcall	deselect
	cbi	PORT_CS
	rjmp	rcv_spi
.endfunc



;---------------------------------------------------------------------------;
; Deselect MMC
;
; void deselect (void);

.global deselect
.func deselect
deselect:
	sbi	PORT_CS
	; Goto next function
.endfunc



;---------------------------------------------------------------------------;
; Receive a byte
;
; BYTE rcv_spi (void);

.global rcv_spi
.func rcv_spi
rcv_spi:
	ldi	r24, 0xFF	; Send 0xFF to receive data
	; Goto next function
.endfunc



;---------------------------------------------------------------------------;
; Transmit a byte
;
; void xmit_spi (BYTE);

.global xmit_spi
.func xmit_spi
xmit_spi:
	ldi	r25, 8
1:	sbrc	r24, 7		; DI = Bit to sent
	sbi	PORT_DI		;
	sbrs	r24, 7		; 
	cbi	PORT_DI		; /
	lsl	r24		; Get DO from MMC
	sbic	PIN_DO		; 
	inc	r24		; /
	sbi	PORT_CK		; A positive pulse to SCLK
	cbi	PORT_CK		; /
	dec	r25		; Repeat 8 times
	brne	1b		; /
	ret
.endfunc
