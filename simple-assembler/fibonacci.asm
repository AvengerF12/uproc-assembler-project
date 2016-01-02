// Fibonacci sequence program

MOV [DD], 0	;
MOV [DE], 1	;

label:

ADD [DD], [DE]  ;
ADD [DE], [DD]  ;

JMP label	;

END             ;

