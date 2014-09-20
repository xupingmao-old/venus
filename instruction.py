

# instructions 


def emit(ins, val = None):
	if val:
		print(ins + ' ' + str(val))
	else:
		print(ins)

def emit_load( v ):
	if v.type == 'string':
		print('LOAD ' + '"' + v.val + '"')
	else:
		print('LOAD ' + str(v.val))

def emit_store(v ):
	emit(STORE, v)

# compute
ADD = 'ADD'
SUB = 'SUB'
MUL = 'MUL'
DIV = 'DIV'
MOD = 'MOD'

# compare
GT = 'GT'
LT = 'LT'
GTEQ = 'GTEQ'
LTEQ = 'LTEQ'
EQEQ = 'EQEQ'
NOTEQ = 'NOTEQ'
IN = 'IN'

# memory
SET = 'SET'
GET = 'GET'
STORE = 'STORE'
LOAD = 'LOAD'
POP = 'POP'

# data
LIST = 'LIST'

# jump
JUMP = 'JUMP'
JUMP_ON_FALSE = 'JUMP_ON_FALSE'
TAG = 'TAG'

# function
CALL = 'CALL'
DEF = 'DEF'
RETURN = 'RETURN'
LOAD_PARAMS = 'LOAD_PARAMS'
EOF = 'EOF'
