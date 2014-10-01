if 'tinytm' not in globals():
    from boot import *

# instructions 

# constants as list
class Constants:
	def __init__(self):
		self.values = []
	def add( self, v):
		if v.type == 'number':
			v.val = float(v.val)
		if v.val not in self.values:
			self.values.append( v.val)
	def load( self, v):
		if v.val not in self.values:
			self.add(v)
		idx = self.values.index( v.val )
		emit(LOAD_CONSTANT, idx)
	def index(self, v):
		self.add(v)
		return self.values.index(v.val)
	def get( self, idx ):
		return self.values[idx]
	def __str__(self):
		for i in self.values:
			print(i)
class Scope:
	def __init__(self):
		self.locals = []
		self.g = []
	def def_global(self, v):
		if v not in self.cg:
			self.g.append(v)
class Names:
	def __init__(self):
		self.scope = Scope()
		self.scopes = []
		self.scopes.append( self.scope )
	def open(self):
		self.scope = Scope()
		self.scopes.append( self.scope )
	def close(self):
		self.scopes.pop()
	def add_global(self, v):
		constants.add(v)
	def add_local(self, v):
		if v.val not in self.scope.locals:
			self.scope.locals.append(v.val)
	def load(self, v):
		if len( self.scopes ) == 0:
			idx = constants.index(v)
			emit(LOAD_GLOBAL, idx)
		elif v.val in constants.values:
			idx = constants.index(v)
			emit(LOAD_GLOBAL, idx)
		else:
			self.add_local(v)
			idx = self.scope.locals.index(v.val)
			emit(LOAD_LOCAL, idx)
	def store(self, v):
		if v.val in self.scope.locals:
			idx = self.scope.locals.index(v.val)
			emit(STORE_LOCAL, idx)
		else:
			idx = constants.index(v)
			emit(STORE_GLOBAL, idx)

constants = Constants()
names = Names()
def emit(ins, val = None):
	if val != None:
		# val = '(' + str(constants.get(val)) + ')'
		print( codes[ins] + ' ' + str(val) )
	else:
		print( codes[ins])

def emit_load( v ):
	t = v.type
	if t == 'string' or t == 'number':
		constants.load( v )
	elif t == 'None':
		emit(NONE)
	elif t == 'name':
		names.load( v )
	else:
		print('LOAD_LOCAL ' + str(v.val))

def def_local( v ):
	names.add_local(v)

def open_scope():
	names.open()

def close_scope():
	names.close()

def emit_store( v ):
	names.store(v)


def def_global( v ):
	names._def(v)

def print_constants():
	for i in constants.values:
		print(i)
	# for i in range( len( constants )) :
	# 	print("%s : (%s, %s)" % ( constants_keys[i], constants[i].type, constants[i].val))

#constants
NONE = 5

# compute
ADD =11
SUB = 12
MUL = 13
DIV = 14
MOD = 15

# compare
GT = 20
LT = 21
GTEQ = 22
LTEQ = 23
EQEQ = 24
NOTEQ = 25
IN = 26
NOTIN = 27
AND = 28
OR = 29

# memory
SET = 30
GET = 31
STORE_LOCAL = 32
STORE_GLOBAL = 33
LOAD_LOCAL = 34
LOAD_GLOBAL = 35
POP = 36
LOAD_CONSTANT = 37

# data
LIST = 40
DICT = 41

# jump
JUMP = 50
POP_JUMP_ON_FALSE = 51
POP_JUMP_ON_TRUE = 52
JUMP_ON_FALSE = 53
JUMP_ON_TRUE = 54
TAG = 55

# function
CALL = 60
DEF = 61
RETURN = 62
LOAD_PARAMS = 63
EOF = 64

codes = {
	LOAD_CONSTANT : "LOAD_CONSTANT",
	LOAD_LOCAL : "LOAD_LOCAL",
	STORE_LOCAL : "STORE_LOCAL",
	LOAD_GLOBAL : "LOAD_GLOBAL",
	STORE_GLOBAL : "STORE_GLOBAL",
	POP : "POP",
	CALL : "CALL",
	ADD : "ADD",
	MUL : "MUL",
	RETURN : "RETURN",
	DEF : "DEF",
	GET : "GET",
	SET : "SET",
	LOAD_PARAMS : "LOAD_PARAMS",
	EOF : "EOF"
}