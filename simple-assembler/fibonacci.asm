// Fibonacci sequence program

START           ;

MOV [DD], 0	;
MOV [DE], 1	;

MOV [DD], [DE]  ;
MOV [DE], [DD]  ;

JMP 6		    ;

END             ;

