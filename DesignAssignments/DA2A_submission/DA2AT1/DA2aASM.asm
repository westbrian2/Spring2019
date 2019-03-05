.org 0

;use PORTB2 for wave output pin 
waveGen:
	ldi r20,4
	out ddrb, r20 ; setting PB2 as output 

	ldi r21, 0; used to turn pin off
	
	ldi r22, 5 ; Prescaler= 1024 
	sts tccr1b, r22

waveBegin: ;Beginning the wave generation
	sts tcnt1h, r21; setting counter to 0
	sts tcnt1l,r21
	out portb,r20; Toggles on
	rcall delayOn ; Creating on of 60% or .435sec
	sts tcnt1h,r23; reset counter
	sts tcnt1l,r23
	out portb,r21 ; turns pin off
	rcall delayOff; creating off of 40% (.29sec)
	jmp waveBegin; creating next period

delayOn: //60% on
	lds r25,tcnt1h; loading upper bits for comparison 
	lds r26,tcnt1l
	cpi r26, 0x8C; checking lower bits
	brsh upperOn
	rjmp delayOn ; loops back to keep checking 

upperOn:
	cpi r25,0x1A ; checking upper bits
	brlt delayOn
	ret

delayOff: //40% off 
	lds r25,tcnt1h
	lds r26,tcnt1l
	cpi r26,0xB3 ; checking lower bits
	brsh upperOff
	rjmp delayOff

upperOff:
	cpi r25,0x11; checking upper bits
	brlt delayOff
	ret