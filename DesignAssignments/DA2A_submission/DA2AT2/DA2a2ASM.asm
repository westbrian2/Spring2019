.org 0

	cbi ddrc,2; make c2 an input
	sbi portc,2; pull up 
	ldi r16,0x00;
	out ddrb, r16; disable output
	sbi ddrb,2; sets pb2 as out
	ldi r20,5 ; prescaler 1024
	sts tccr1b,r20; 
	ldi r20,0 

	delayReset: 
		sbi portb,2 ; reset led
		sts tcnt1h,r20; counter = 0
		sts tcnt1l,r20; 
	button:
		sbic pinc,2 ; checking if C2 is 0
		jmp button ;continures to check if c2 is 1
		cbi portb,2 ; lights led
		jmp delay; sets delay 
;DELAY SUBROUTINE*******************************************************
	delay:
		lds r21,tcnt1h; getting upper bits from counter
		lds r22,tcnt1l; getting lower
		cpi r22,0x4B; comparing against target value
		brsh upper
		jmp delay ; if target value isn't met restart loop
	upper:
		cpi r21,0x4C
		brlt delay

	end:
		sbi portb,2; turn back off
		jmp end