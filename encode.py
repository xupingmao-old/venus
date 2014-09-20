# encode for

from parse import *
from instruction import *


def store(t):
    if t.type == 'name':
        emit_store( t )
    elif t.type == 'get':
        encode_item(t.b)
        encode_item(t.a)
        emit(SET)
    elif t.type == ',':
        store(t.b)
        store(t.a)

tag_count = 0
def newtag():
    global tag_count
    tag_count+=1
    return tag_count

def jump( p, ins = JUMP):
    emit( ins, p)

def tag( p):
    emit(TAG, p)

op_map = {
    '+':ADD,
    '-':SUB,
    '*':MUL,
    '/':DIV,
    '%':MOD,
    '>' : GT,
    '<' : LT,
    '>=': GTEQ,
    '<=': LTEQ,
    '==': EQEQ,
    '!=': NOTEQ,
    'get': GET,
    'in' : IN,
    'notin' : NOTIN
}
op_list = op_map.keys()
op_ext_map = {
    '+=' : ADD,
    '-=' : SUB,
    '*=' : MUL,
    '/=' : DIV,
    '%=' : MOD
}
op_ext_list = op_ext_map.keys()


def encode_item( tk ):
    if tk == None: return 0
    if isinstance(tk, list):
        for i in tk:
            encode_item(i)
            if i.type == '$': emit(POP)
        return
    t = tk.type
    if t == '=':
        encode_item(tk.b)
        store(tk.a)
    elif t == ',':
        return encode_item( tk.a ) + encode_item(tk.b)
    elif tk.type == 'list':
        n = encode_item(tk.val)
        emit(LIST, n)
    elif t == '$':
        encode_item(tk.name)
        n = encode_item(tk.args)
        emit(CALL , n)
    elif t == 'arg':
        if tk.val:
            encode_item(tk.val)
            store(tk.name)
        #emit('set_arg '+tk.name)
    elif t == 'def':
        emit(DEF)
        encode_item(tk.args)
        emit(LOAD_PARAMS)
        encode_item(tk.body)
        emit(EOF)
        store(tk.name)
    elif t == 'return':
        if tk.val:encode_item(tk.val)
        emit(RETURN)
    elif t == 'if':
        encode_item(tk.cond)
        else_tag,end_tag = newtag(), newtag()
        jump(else_tag, POP_JUMP_ON_FALSE)
        encode_item(tk.left)
        jump(end_tag)
        tag(else_tag)
        encode_item(tk.right)
        tag(end_tag)
    elif t == 'from':
        encode_item(Token('name','importfrom'))
        n = encode_item(tk.a)
        n += encode_item(tk.b)
        emit(CALL, n)
        emit(POP)
    elif t in op_list:
        encode_item(tk.a)
        encode_item(tk.b)
        emit( op_map[t])
    elif t in op_ext_list:
        encode_item(tk.b)
        encode_item(tk.a)
        emit( op_ext_map[t] )
        store(tk.a)
    elif t == 'and':
        end = newtag()
        encode_item(tk.a)
        emit(JUMP_ON_FALSE, end)
        encode_item(tk.b)
        emit(AND)
        tag( end )
    elif t == 'or':
        end = newtag()
        encode_item(tk.a)
        emit( JUMP_ON_TRUE, end)
        encode_item( tk.b )
        emit(OR)
        tag( end )
    elif t in  [ "and", "or", "for","while"]:
        encode_item(tk.a)
        encode_item(tk.b)
        emit(tk.type)
        return 1
    elif t in ['number', 'name', "string", 'None']:
        emit_load( tk )
        return 1
    else:
        pass


def encode(content):
    r = parse(content)
    encode_item(r)


def main( ):
    import sys
    name = 'test1.py'
    if len(sys.argv) > 1:
        name = sys.argv[1]
    encode( open(name).read() )
    print('\n\n==========constants=============')
    print_constants()
    # input("pause")

main()