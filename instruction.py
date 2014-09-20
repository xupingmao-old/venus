

# instructions 


def emit(ins, val = None):
	if val != None:
		print(ins + ' ' + str(val))
	else:
		print(ins)

def emit_load( v ):
	t = v.type
	if t == 'string':
		load_g( v.val )
	elif t == 'None':
		emit(NONE)
	elif t == 'name':
		if v.val in cur_scope.g:
			load_g( v.val )
		elif v.val in cur_scope.locals:
			load_l( v.val )
		else:
			load_g( v.val )
	else:
		print('LOAD ' + str(v.val))

def load_l( v ):
	if v not in cur_scope.locals:
		cur_scope.locals.append( v )
	emit( LOAD, cur_scope.locals.index(v) )
def load_g( v ):
	if v not in constants:
		constants.append(v)
	emit( LOADG, constants.index(v))

def store_g( v ):
	if v not in constants:
		constants.append(v)
	emit( STOREG, constants.index(v))

def store_l( v ):
	if v not in cur_scope.locals:
		cur_scope.locals.append( v )
	emit( STORE, cur_scope.locals.index(v) )

def emit_store( v ):
	if len( scopes ) == 1:
		store_g( v.val )
	elif v.val in cur_scope.g:
		store_g( v.val )
	else:
		store_l( v.val )

class Scope:

	def __init__(self):
		self.locals = []
		self.g = []

	def def_global(self, v):
		if v not in self.cg:
			self.g.append(v)
scopes = []
cur_scope = Scope()
scopes = [cur_scope]
constants = []

def new_scope():
	cur_scope = Scope()
	scopes.append( cur_scope )

def quit_scope():
	scopes.pop()
	cur_scope = scopes[-1]

def def_global( v ):
	cur_scope.def_global(v)

def print_constants():
	print( constants )

#constants
NONE = 'None'

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
NOTIN = 'NOTIN'
AND = 'AND'
OR = 'OR'

# memory
SET = 'SET'
GET = 'GET'
STORE = 'STORE'
STOREG = 'STOREG'
LOAD = 'LOAD'
LOADG = 'LOADG'
POP = 'POP'

# data
LIST = 'LIST'

# jump
JUMP = 'JUMP'
POP_JUMP_ON_FALSE = "POP_JUMP_ON_FALSE"
POP_JUMP_ON_TRUE = 'POP_JUMP_ON_TRUE'
JUMP_ON_FALSE = 'JUMP_ON_FALSE'
JUMP_ON_TRUE = 'JUMP_ON_TRUE'
TAG = 'TAG'

# function
CALL = 'CALL'
DEF = 'DEF'
RETURN = 'RETURN'
LOAD_PARAMS = 'LOAD_PARAMS'
EOF = 'EOF'
