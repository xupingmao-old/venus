if 'tinytm' not in globals():
    from boot import *
# encode for

from parse import *
from instruction import *

def store(t):
    if t.type == 'name':
        emit_store( t )
    elif t.type == 'get':
        encode_item(t.a)
        encode_item(t.b)
        emit(SET)
    elif t.type == ',':
        store(t.b)
        store(t.a)

tag_count = 0
def newtag():
    global tag_count
    tag_count+=1
    return tag_count-1

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

start_tag_list = [-1]
end_tag_list = [-1]

in_class = False

def print_tok_pos( tk ):
    if isinstance(tk, Token):
        print(tk.pos)
    elif hasattr(tk, 'a'):
        print_tok_pos(tk.a)
    elif hasattr(tk, 'val'):
        print_tok_pos(tk.val)

def build_set(self, key, val):
    node = AstNode('=')
    node.b = val
    setNode = AstNode('get')
    setNode.a = key
    setNode.b = val
    node.a = setNode
    return node

def encode_item( tk ):
    global in_class
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
    elif t == 'neg':
        if tk.val.type == 'number':
            tk.type = 'number'
            tk.val = -tk.val.val
        emit_load(tk)
    elif t == 'arg':
        def_local(tk.name)
        if tk.val:
            encode_item(tk.val)
            store(tk.name)
        #emit('set_arg '+tk.name)
    elif t == 'def':
        emit(TM_DEF)
        open_scope()
        encode_item(tk.args)
        emit(LOAD_PARAMS)
        encode_item(tk.body)
        emit(TM_EOF)
        close_scope()
        if not in_class:
            emit_store(tk.name)
    elif t == 'class':
        in_class = True
        buildclass = []
        emit(DICT, 0)
        store(tk.name)
        for func in tk.body:
            if func.type != 'def':
                raise "at " + func.pos + " do not support non-func expression in class"
            encode_item(func)
            emit_load(tk.name)
            emit_load_str(func.name)
            emit(SET)
        in_class = False
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
    elif t == 'while':
        start_tag, end_tag = newtag(), newtag()
        # set while stack
        start_tag_list.append( start_tag )
        end_tag_list.append( end_tag )
        ###
        tag(start_tag)
        encode_item(tk.a)
        jump(end_tag, POP_JUMP_ON_FALSE)
        encode_item(tk.b)
        jump(start_tag)
        tag(end_tag)
        # clear while stack
        start_tag_list.pop()
        end_tag_list.pop()
    elif t == 'break':
        jump( end_tag_list [-1] )
    elif t == 'continue':
        jump( start_tag_list[-1] )
    elif t == 'from':
        encode_item(Token('name','importfrom'))
        n = encode_item(tk.a)
        n += encode_item(tk.b)
        emit(CALL, n)
        emit(POP)
    elif t == "get":
        encode_item(tk.a)
        encode_item(tk.b)
        emit( GET )
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
        tag( end )
    elif t == 'or':
        end = newtag()
        encode_item(tk.a)
        emit( JUMP_ON_TRUE, end)
        encode_item( tk.b )
        tag( end )
    elif t == "for":
        start_tag, end_tag = newtag(), newtag()
        # set for stack
        start_tag_list.append( start_tag )
        end_tag_list.append( end_tag )
        ### load index and iterator
        encode_item( tk.a.b )
        load_number(0)
        tag(start_tag)
        jump(end_tag, TM_FOR)
        # store the next value to a, if in func , store to locals
        #if inlocal():
            #def_local(tk.a.a)
        # not need anymore, because we will assume the var to be locals
        # if it is not in globals of the scope
        store( tk.a.a )
        encode_item(tk.b)
        jump(start_tag)
        tag(end_tag)
        # clear for stack
        start_tag_list.pop()
        end_tag_list.pop()
        emit(POP)
        emit(POP)
    elif t in ('number', 'name', "string", 'None'):
        emit_load( tk )
    elif t == 'global':
        def_global(tk.val)
    else:
        pass
    return 1


def encode(content):
    r = parse(content)
    encode_item(r)


def main( ):
    import sys
    name = 'test1.py'
    if len( argv ) == 2:
        name = argv[1]
    elif len(argv) == 3 and argv[1] == '-save':
        name = argv[2]
    encode( load(name) )
    save_code("bin", tag_count)
    print('\n\n==========constants=============')
    print_constants()
    # input("pause")

main()
