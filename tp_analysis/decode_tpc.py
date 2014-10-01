
from boot import *

EOF=0
ADD=1
SUB=2
MUL=3
DIV=4
POW=5
AND=6
OR=7
CMP=8
GET=9
SET=10
NUMBER=11
STRING=12
GGET=13
GSET=14
MOVE=15
DEF=16
PASS=17
JUMP=18
CALL=19
RETURN=20
IF=21
DEBUG=22
EQ=23
LE=24
LT=25
DICT=26
LIST=27
NONE=28
LEN=29
POS=30
PARAMS=31
IGET=32
FILE=33
NAME=34
NE=35
HAS=36
RAISE=37
SETJMP=38
MOD=39
LSH=40
RSH=41
ITER=42
DEL=43
REGS=44

code4 = {
    NAME : 'NAME',
    FILE : "FILE",
    GGET : "GGET",
    GSET : "GSET",
    PARAMS : "PARAMS",
    CALL : "CALL",
    GET : "GET",
    SET : "SET",
    HAS : "HAS",
    IF : "IF",
    MOVE : "MOVE",
    REGS : "REGS",
    RETURN : "RETURN",
    LIST : "LIST",
    DICT: "DICT",
    IGET : "IGET",
    ADD : "ADD",
    SUB : "SUB",
    MUL : "MUL",
    DIV : "DIV",
    MOD : "MOD",
    AND : "AND",
    RSH : "RSH",
    EQ : "EQ",
    RAISE : "RAISE",
    ITER : "ITER"
}

code1 = {
    NONE: "None",
    EOF : "EOF"
}

data = {
    STRING: "STRING"
}

code2 = {
    JUMP : "JUMP",
    SETJMP : "SETJMP",
    DEF : "DEF"
}

def main():
    name = 'encode.tpc'
    if len(argv) > 1:
        name = argv[1]
    bin = load(name)
    l = len(bin)
    i = 0
    while i < l:
        op = ord(bin[i])
        b = ord(bin[i+1])
        c = ord(bin[i+2])
        d = ord(bin[i+3])
        i+=4
        if op == NUMBER:
            i+=8
            print("NUMBER " +str(b))
        elif op in data:
            olen = short( c, d )
            s = bin[i: i + olen]
            i += int( olen / 4 ) * 4 + 4
            print( data[op] + ' %d "' % b + s + '"')
        elif op == POS:
            olen = b * 4
            s = bin[i: i + olen - 1]
            i+= olen
            print('POS "'+ s + '"')
        elif op in code4:
            print( code4[op] + " %d, %d, %d" %(b,c,d))
        elif op in code2:
            print( code2[op] + " %d" % short(c, d))
        elif op in code1:
            print( code1[op] )
        else:
            print(op)

main()


