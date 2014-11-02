from boot import *
from tokenize import *
#constants
# NONE = 5
NEW_STRING = 1
NEW_NUMBER = 2
TM_FILE = 3

# compute
ADD =11
SUB = 12
MUL = 13
DIV = 14
MOD = 15
NEG = 16
NOT = 17

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
DICT_SET = 38

# data
LIST = 40
DICT = 41
LIST_APPEND = 42

# jump
JUMP = 50
POP_JUMP_ON_FALSE = 51
POP_JUMP_ON_TRUE = 52
JUMP_ON_FALSE = 53
JUMP_ON_TRUE = 54
TAG = 55
TAGSIZE = 56
TM_FOR = 57
SETJUMP = 58

# function
CALL = 60
TM_DEF = 61
RETURN = 62
LOAD_PARAMS = 63
TM_EOF = 64
#end of program
TM_EOP = 65 

codes = {
    NEW_STRING: "NEW_STRING",
    NEW_NUMBER: "NEW_NUMBER",
    LOAD_CONSTANT : "LOAD_CONSTANT",
    LOAD_LOCAL : "LOAD_LOCAL",
    TM_FILE : "TM_FILE",
    STORE_LOCAL : "STORE_LOCAL",
    LOAD_GLOBAL : "LOAD_GLOBAL",
    STORE_GLOBAL : "STORE_GLOBAL",
    POP : "POP",
    CALL : "CALL",
    ADD : "ADD",
    MUL : "MUL",
    SUB : "SUB",
    DIV : "DIV",
    GT : "GT",
    LT : "LT",
    NEG: "NEG",
    GTEQ: "GTEQ",
    LTEQ: "LTEQ",
    EQEQ: "EQEQ",
    NOTEQ: "NOTEQ",
    AND: "AND",
    OR: "OR",
    RETURN : "RETURN",
    TM_DEF : "TM_DEF",
    GET : "GET",
    SET : "SET",
    LOAD_PARAMS : "LOAD_PARAMS",
    TM_EOF : "TM_EOF",
    TM_EOP : "TM_EOP",
    LIST : "LIST",
    DICT : "DICT",
        LIST_APPEND : "LIST_APPEND",
    EQEQ : "EQEQ",
    POP_JUMP_ON_TRUE : "POP_JUMP_ON_TRUE",
    POP_JUMP_ON_FALSE : "POP_JUMP_ON_FALSE",
    JUMP_ON_TRUE : "JUMP_ON_TRUE",
    JUMP_ON_FALSE: "JUMP_ON_FALSE",
    IN : "IN",
    NOTIN: "NOTIN",
    TAG : "TAG",
    JUMP : "JUMP",
    TAGSIZE : "TAGSIZE",
    TM_FOR : "TM_FOR",
    SETJUMP : "SETJUMP",
    NOT : "NOT",
    DICT_SET: "DICT_SET"
}
# instructions 

def code(type, val):
    if istype(val, "string"):
        return chr(type) + code16(len(val))+ val
    elif istype(val, "number"):
        return chr(type) + codeF(val)

# constants as list
class Constants:
    def __init__(self):
        self.values = [None]
    def add( self, v):
#        if v.type == 'number':
#            v.val = float(v.val)
#        elif v.type == 'None':
#            pass
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
        self.globals = []
    def def_global(self, v):
        if v not in self.globals:
            self.globals.append(v)
class Names:
    def __init__(self):
        self.scope = Scope()
        self.scopes = [self.scope]
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
    def get_local(self, lc):
        return self.scope.locals.index(lc.val)
    def load(self, v):
        # same as store, check scope level first
        if len( self.scopes ) == 1:
            idx = constants.index(v)
            emit(LOAD_GLOBAL, idx)
        # check locals
        elif v.val not in self.scope.locals:
            idx = constants.index(v)
            emit(LOAD_GLOBAL, idx)
        else:
            idx = self.scope.locals.index(v.val)
            emit(LOAD_LOCAL, idx)
    def indexlocal(self, v):
        if v.val not in self.scope.locals:
            self.scope.locals.append(v.val)
        return self.scope.locals.index(v.val)
    def store(self, v):
        # first ,check scope level
        if len(self.scopes) == 1:
            idx = constants.index(v)
            emit(STORE_GLOBAL, idx)
        # check if in globals defined in the function, 
        # or store as local
        elif v.val not in self.scope.globals:
            idx = self.indexlocal(v)
            emit(STORE_LOCAL, idx)
        else:
            idx = constants.index(v)
            emit(STORE_GLOBAL, idx)

constants = None
names = None
bin = None
out = None

def ins_init():
    global constants
    global names
    global bin
    global out

    constants = Constants()
    names = Names()
    bin = "" # binary code
    out = []

ins_init()
def def_global( v ):
    names.scope.globals.append(v.val)
# opcode : op
mode1 = [ADD, SUB, MUL, DIV, MOD, NEG, NOT, POP, GET, SET, AND, OR,
        LT, GT, LTEQ, GTEQ, EQEQ, NOTEQ,IN, NOTIN,
    TM_EOF, TM_EOP, RETURN, LOAD_PARAMS,LIST_APPEND, DICT_SET]
# opcode : op byte
mode2 = [LOAD_LOCAL,STORE_LOCAL, CALL, LIST, DICT]
# opcode : op short
mode3 = [LOAD_GLOBAL, STORE_GLOBAL, LOAD_CONSTANT, TM_FILE, TAG, TAGSIZE,
JUMP_ON_TRUE, JUMP_ON_FALSE,
POP_JUMP_ON_TRUE, POP_JUMP_ON_FALSE, JUMP, TM_FOR, TM_DEF, SETJUMP]

def code_pos():
    return len(out) - 1

def load_number(v):
    tk = Token("number", v)
    constants.load(tk)
#    lc = Token("name", "#" + str(len(names.scope.locals)))
#    names.add_local(lc)
#    names.store( lc )

def local_new( v):
    if istype(v, 'number'):
        lc = '#'+str(names.scope.locals)
        lc = Token( 'number', lc)
        names.add_local( lc )
        constants.load( Token("number", v))
        names.store( lc )
        return lc

def emit_iter( lc, jmp):
    global bin
    code8( TM_FOR )
    code8( names.get_local(lc) )
    code16( jmp )
    print("TM_FOR " + str(names.get_local(lc)) + " " + str(jmp))

def batch_jmp( pos ):
    out[pos][1] = code_pos() - pos

def emit(ins, val = 0):
    # out.append([ins,val])
    global bin
    if ins in mode1:
        bin += code8(ins)
    elif ins in mode2:
        bin += code8(ins) + code8(val)
    elif ins in mode3:
        bin += code8(ins) + code16(val)
    elif ins == NEW_STRING:
        bin += code(ins, val)
    elif ins == NEW_NUMBER:
        bin += code(ins, val)
    # if ins == NEW_STRING:
    #     print( codes[ins] + '['+str(len(val))+']'+str(val))
    # elif val != None:
    #     # val = '(' + str(constants.get(val)) + ')'
    #     print( codes[ins] + ' ' + str(val) )
    # else:
    #     print( codes[ins])

def emit_def( v):
    idx = constants.index(v)
    emit(TM_DEF, idx)

def emit_load( v ):
    if v == None:
        emit(LOAD_CONSTANT, 0)
        return;
    t = v.type
    if t == 'string' or t == 'number':
        constants.load( v )
    elif t == 'None':
        emit(LOAD_CONSTANT, 0)
    elif t == 'name':
        names.load( v )
    else:
        print('LOAD_LOCAL ' + str(v.val))

def emit_load_g(v):
    constants.load(v)

def emit_load_str(v):
    constants.load(v)

def emit_load_None():
    emit(LOAD_CONSTANT, 0)

def gen_code(tagsize):
    emit(TM_EOP)
    global bin
    code = bin
    bin = ""
    for i in constants.values:
        if istype(i, "string"):
            emit(NEW_STRING, i)
        elif istype(i, "number"):
            emit( NEW_NUMBER, i)
    emit( TAGSIZE, tagsize)
    return bin + code

def inlocal():
    return len(names.scopes) > 1

def def_local( v ):
    names.add_local(v)

def open_scope():
    names.open()

def close_scope():
    names.close()

def emit_store( v ):
    names.store(v)


def print_constants():
    for i,k in enumerate(constants.values):
        print(i,k)
    # for i in range( len( constants )) :
    #     print("%s : (%s, %s)" % ( constants_keys[i], constants[i].type, constants[i].val))

