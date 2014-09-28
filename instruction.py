

# instructions 


def emit(ins, val = None):
	if val != None:
		print(ins + ' ' + str(val))
	else:
		print(ins)

def emit_load( v ):
	t = v.type
	if t == 'string' or t == 'number':
		load_g( v )
	elif t == 'None':
		emit(NONE)
	elif t == 'name':
		if v.val in cur_scope.g:
			load_g( v )
		elif v.val in cur_scope.locals:
			load_l( v.val )
		else:
			load_g( v )
	else:
		print('LOAD_LOCAL ' + str(v.val))

def def_local( v ):
	if v not in cur_scope.locals:
		cur_scope.locals.append( v )

def load_l( v ):
	if v not in cur_scope.locals:
		cur_scope.locals.append( v )
	emit( LOAD_LOCAL, cur_scope.locals.index(v) )
def load_g( v ):
	if v.val not in constants_keys:
		constants_keys.append(v.val)
		constants.append( v )
	emit( LOAD_GLOBAL, constants_keys.index(v.val))

def store_g( v ):
	if v.val not in constants_keys:
		constants_keys.append(v.val)
		constants.append( v )
	emit( STORE_GLOBAL, constants_keys.index(v.val))

def store_l( v ):
	if v not in cur_scope.locals:
		cur_scope.locals.append( v )
	emit( STORE_LOCAL, cur_scope.locals.index(v) )

def emit_store( v ):
	# the scope is global
	if len( scopes ) == 1:
		store_g( v )
	elif v.val in cur_scope.g:
		store_g( v )
	else:
		store_l( v )

class Scope:

	def __init__(self):
		self.locals = []
		self.g = []

	def def_global(self, v):
		if v not in self.cg:
			self.g.append(v)

cur_scope = Scope()
scopes = [cur_scope]
constants = []
constants_keys = []


def new_scope():
	cur_scope = Scope()
	scopes.append( cur_scope )

def quit_scope():
	scopes.pop()
	cur_scope = scopes[-1]

def def_global( v ):
	cur_scope.def_global(v)

def print_constants():
	for i in range( len( constants )) :
		print("%s : (%s, %s)" % ( constants_keys[i], constants[i].type, constants[i].val))

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
STORE_LOCAL = 'STORE_LOCAL'
STORE_GLOBAL = 'STORE_GLOBAL'
LOAD_LOCAL = 'LOAD_LOCAL'
LOAD_GLOBAL = 'LOAD_GLOBAL'
POP = 'POP'

# data
LIST = 'LIST'
DICT = 'DICT'

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
