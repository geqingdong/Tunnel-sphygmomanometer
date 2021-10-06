$NOMOD51
;------------------------------------------------------------------------------
;
;  Copyright (c) 2015 SONiX Technology Co., Ltd.
;  Version 1.0 - SZC909, SZC908
;
;  *** <<< Use Configuration Wizard in Context Menu >>> ***
;------------------------------------------------------------------------------
;
;  This preference, such as watchdog, external reset pin, and clock source, is preloaded 
;  during the microcontroller's power-on. It is strongly recommanded to use configuration 
;  wizard to set these parameters up appropriately.
;
;------------------------------------------------------------------------------
ROM_SIZE EQU 0XFFFF

;   <o> Program Memory Security <0x00=> Enable <0x01=> Disable
	SECURITY_SET    EQU     0x01	;	{0x3FFF}
;   <i> The debug interface cannot read program memory if this security option is enable.
;   <i> Erase All Program Memory Action can be proformmed.
;
;   <o.1> CPU Clock Source <0x00=> IHRC_RTC <0x01=> IHRC 
    CLOCKSRC_SET    EQU     0x02	;	{0x3FFF}
;   <i> IHRC: P20/P21 as IO function.
;   <i> IHRC_RTC: P20/P21 connect 32768Hz X'tal.
;
;   <o> Noise Filter <0x01=> Disable <0x00=> Enable
    NOISEFILT_SET   EQU     0x00	;	{0x3FFC}

;
;   <o.4..7> Watchdog <0x00=> Always <0x05=> Enable <0x0A=> Disable
    WATCHDOG_SET    EQU     0xA0	;	{0x3FFF}
;   <i> Always: Trun on watchdog function including Normal, IDLE, and SLEEP mode.
;   <i> Enable: Turn on watchdog function only in Normal mode.
;   <i> Disable: Turn off watchdog function.
;
;   <o.6..7> Watchdog Overflow Period <0x00=> 64 ms <0x01=> 128 ms <0x02=> 256 ms <0x03=> 512 ms
    WATCHCLK_SET    EQU     0xC0
;   <i> The watchdog overflow period is based on Internal Low R-C Clock which has a gentle inaccuracy.


    CSEG    AT      0xFFFB		
	DB      0xF8 + NOISEFILT_SET + 0x02 				
    DB      0x3E + WATCHCLK_SET + NOISEFILT_SET
    DB      0x5A
    DB      0xA5
    DB      0x0C + WATCHDOG_SET + CLOCKSRC_SET + SECURITY_SET
    END