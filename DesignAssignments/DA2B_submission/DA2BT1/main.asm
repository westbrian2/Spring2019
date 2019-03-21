.include <m328pdef.inc>
.org 0 ; body of program
jmp main ; 
.org 2 ; external interrupt
jmp EX0_ISR
main:
	;setting up stack
	ldi r20,high(RAMEND)
	out sph,r20
	ldi r20,low(RAMEND)
	out spl, r20
	;setting up timer 
	ldi r20,4 ; prescaler 1024
	sts tccr1b,r20;

	ldi r20,0x2;INT0 is falling edge triggered
	sts EICRA,r20
	sbi ddrb,2; portb2 is output
	sbi portb,2; turning off led 
	sbi portd,2; pull up resistor
	ldi r20,1<<INT0; enables INT0
	out EIMSK,r20
	sei; enables interrupts
	end:
	jmp end

delay:
		lds r22,tcnt1h; getting upper bits from counter
		lds r23,tcnt1l; getting lower
		cpi r23,0x25; comparing against target value
		brsh upper
		jmp delay ; if target value isn't met restart loop
	upper:
		cpi r22,0x26
		brlt delay
		ret
EX0_ISR:
	 ldi r20,0
	 sts tcnt1h,r20; resetting timer
	 sts tcnt1l,r20; 
	 in r21,portb; 
	 ldi r22,(1<<2); 
	 eor r21,r22; toggles LED
	 out portb,r21; 
	 rcall delay ; starting delay
	 eor r21,r22
	 out portb,r22; 
	 reti ; returns 