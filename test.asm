.text	
	        li $r1, 0xBB8
		str $r1, 1 ($r29)
		li $r1, 0x0
		str $r1, 2 ($r29)
LABEL_4:	li $r1, 0x1
		li $r20, 1, 
		jeq $r1, $r20, LABEL_6
		jmp LABEL_5
LABEL_6:	lod $r1, 2 ($r29)
		not $r1, $r1, $r1
		str $r1, 2 ($r29)
		li $r1, 0x3e8
		li $r2, 0x0
LABEL_15:	subi $r1, $r1, 0x1
		jge $r1, $r2, LABEL_15
		jmp LABEL_4
LABEL_5: