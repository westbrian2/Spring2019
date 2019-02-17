// Brian West Design Assignment 1
start:
    ldi r25,0xFF ; high 8 of M
	ldi r24,0xFF ; low 8 of M
	ldi r22,0xAA ; multiplier (m)
	ldi r18,0; [7:0] answer
	ldi r19,0;[15:8] answer
	ldi r20,0;[23:16]answer
	ldi r16,0; Carry trasfer register(0+carry=carry) 

	multiply: 
	cp r22,r15; compares multiplier with 0, if not 0 continue
	breq end  ; jumps to end if 0 
	add r18,r24	;adding low of M into answer [7:0] 
	adc r19,r25 ;adding high of M into answer [15:8] with carry from low
	adc r20,r16 ; placing carry bit into answer[23:16]
	dec r22 ; dec m (acts as counter)
	jmp multiply
	
	
	end:
	jmp end
