.CODE
.ADDR x0000
CONST R1 #3
BRp POSITIVE
NOP
POSITIVE
ADD R3 R1 #-7
BRnz NEGATIVE
NOP
NEGATIVE
MUL R1 R3 R1
BRn SUBTRACT
DIV R1 R3 R3
SUBTRACT
SUB R3 R3 R3
BRz ZERO
NOP
ZERO
DIV R1 R3 R1
BRnp WRONG_END
BRnzp END
WRONG_END
MUL R5 R5 R5
END
ADD R5 R5 R5
TRAP x00FF
.OS
.CODE
.ADDR x80FF
HALT
NOP
CONST R7 #0
RTI
