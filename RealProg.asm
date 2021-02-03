;-----------------------------------------------------------------------------------------------------------------
;  Assembly Code Generated from File: ("C:/ProcessorBlock/MyProcessorCompiler/Build/QMake/BaiscProgram.bas") on Fri July 14 17 at 14:53:13 
;-----------------------------------------------------------------------------------------------------------------
;CODE SEGMENT

.text:
	;PROGRAM_MyBasicTestProgram
		;Let Statement Line : 5
		li $r1, 8
		str  $r1, 1 ($r15)
		;Let Statement Line : 6
		li $r1, 12
		str  $r1, 2 ($r15)
		;Let Statement Line : 7
		li $r1, 1024
		str  $r1, 3 ($r15)
		;While Statement Line : 9
LABEL_6:
		li $r1, 1
		li  $r20, 1
		jeq  $r1, $r20, LABEL_8
		jmp  LABEL_7
LABEL_8:
		;Assignment Statement Line : 10
		lod  $r1, 3($r15)
		li $r2, 2
		div  $r1, $r1,$r2
		str $r1, 3($r15)
		jmp LABEL_6
LABEL_7:
;END 	PROGRAM_MyBasicTestProgram


