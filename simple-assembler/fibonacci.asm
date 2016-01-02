// Fibonacci sequence program

MOV [DD], 0	;
MOV [DE], 1	;

label:

MOV [DD], [DE]  ;
MOV [DE], [DD]  ;

JMP label	;

END             ;

