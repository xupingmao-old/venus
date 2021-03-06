from parse import *
from codegen import *

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
    'notin' : NOTIN,
    'get':GET
}
op_ext_map = {
    '+=' : ADD,
    '-=' : SUB,
    '*=' : MUL,
    '/=' : DIV,
    '%=' : MOD
}

start_tag_list = [-1]
end_tag_list = [-1]

in_class = False


def build_set(self, key, val):
    node = AstNode('=')
    node.b = val
    setNode = AstNode('get')
    setNode.a = key
    setNode.b = val
    node.a = setNode
    return node

def encode_op(tk):
    encode_item(tk.a)
    encode_item(tk.b)
    emit( op_map[tk.type])
def encode_op_ext(tk):
    encode_item(tk.a)
    encode_item(tk.b)
    emit( op_ext_map[tk.type] )
    store(tk.a)
def encode_list0(v):
    if v == None: return
    if v.type == ',':
        encode_list0(v.a)
        encode_list0(v.b)
    else:
        encode_item(v)
        emit(LIST_APPEND)
def encode_list(v):
    emit(LIST, 0)
    encode_list0(v.val)

def encode_if(tk):
    encode_item(tk.cond)
    else_tag,end_tag = newtag(), newtag()
    jump(else_tag, POP_JUMP_ON_FALSE)
    encode_item(tk.left)
    jump(end_tag)
    tag(else_tag)
    encode_item(tk.right)
    tag(end_tag)
def encode_assign(tk):
    encode_item(tk.b)
    store(tk.a)
def encode_tuple(tk):
    return encode_item( tk.a ) + encode_item(tk.b)
def encode_dict(tk):
    items = tk.items
    emit(DICT, 0)
    if items != None:
        for item in items:
            encode_item(item[0])
            encode_item(item[1])
            emit(DICT_SET)
def encode_neg(tk):
    if tk.val.type == 'number':
        tk = tk.val
        tk.val = -tk.val
        encode_item(tk)
    else:
        encode_item(tk.val)
        emit(NEG)
def encode_not(tk):
    encode_item(tk.val)
    emit(NOT)
def encode_call(tk):
    encode_item(tk.name)
    n = encode_item(tk.args)
    emit(CALL , n)
def encode_arg(tk):
    def_local(tk.name)
    if tk.val:
        encode_item(tk.val)
        store(tk.name)
def encode_def(tk):
    emit_def(tk.name)
    open_scope()
    encode_item(tk.args)
    emit(LOAD_PARAMS)
    encode_item(tk.body)
    emit(TM_EOF)
    close_scope()
    if not in_class:
        emit_store(tk.name)
def encode_class(tk):
    global in_class
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
def encode_return(tk):
    if tk.val:encode_item(tk.val)
    else:emit_load(None);
    emit(RETURN)
def encode_while(tk):
    start_tag, end_tag = newtag(), newtag()
    # set while stack
    start_tag_list.append( start_tag )
    end_tag_list.append( end_tag )
    ###
    tag(start_tag)
    encode_item(tk.a)
    jump(end_tag, POP_JUMP_ON_FALSE)
    encode_item(tk.b)
    #emit(UP_JUMP, start_tag)
    jump(start_tag)
    tag(end_tag)
    # clear while stack
    start_tag_list.pop()
    end_tag_list.pop()
def encode_continue(tk):
    #emit( UP_JUMP, start_tag_list[-1] )
    jump(start_tag_list[-1])
def encode_break(tk):
    jump( end_tag_list [-1] )
def encode_from(tk):
    encode_item(Token('name','_import'))
    n = encode_item(tk.a)
    n += encode_item(tk.b)
    emit(CALL, n)
    emit(POP)
def encode_import(tk):
    if tk.a.type == ',':
        encode_import(tk.a)
        encode_import(tk.b)
    else:
        encode_item(Token('name','_import'))
        encode_item(tk.a)
        emit(CALL,1)
        emit(POP)
def encode_and(tk):
    end = newtag()
    encode_item(tk.a)
    emit(JUMP_ON_FALSE, end)
    encode_item(tk.b)
    emit(AND)
    tag( end )
def encode_or(tk):
    end = newtag()
    encode_item(tk.a)
    emit( JUMP_ON_TRUE, end)
    encode_item( tk.b )
    emit(OR)
    tag( end )
def encode_for(tk):
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
    #emit(UP_JUMP, start_tag)
    jump(start_tag)
    tag(end_tag)
    # clear for stack
    start_tag_list.pop()
    end_tag_list.pop()
    emit(POP)
    emit(POP)
def encode_global(tk):
    def_global(tk.val)
def encode_try(tk):
    exception = newtag()
    end = newtag()
    emit(SETJUMP, exception)
    encode_item(tk.first)
    jump(end)
    tag(exception)
    encode_item(tk.second)
    tag(end)
    
def do_nothing(tk):
    pass
encode_map = {
    'if': encode_if,
    '=': encode_assign,
    ',': encode_tuple,
    'dict': encode_dict,
    '$': encode_call,
    'neg': encode_neg,
    'not': encode_not,
    'list':encode_list,
    'arg':encode_arg,
    'def':encode_def,
    'class':encode_class,
    'return':encode_return,
    'while':encode_while,
    'continue': encode_continue,
    'break':encode_break,
    'from':encode_from,
    'import':encode_import,
    'and':encode_and,
    'or':encode_or,
    'for':encode_for,
    'global':encode_global,
    'name':emit_load,
    'number':emit_load,
    'string':emit_load,
    'None':emit_load,
    'True':emit_load,
    'False':emit_load,
    'try':encode_try,
    'pass':do_nothing,
}

for k in op_map:
    encode_map[k] = encode_op
for k in op_ext_map:
    encode_map[k] = encode_op_ext

def encode_item( tk ):
    global in_class
    if tk == None: return 0
    if istype(tk, 'list'):
        for i in tk:
            encode_item(i)
            if i.type == '$': emit(POP)
        return
    r = encode_map[tk.type](tk)
    if r:return r
    return 1

load_type_list = ['number', 'name', "string", 'None']

def encode(content):
    global tag_count
    tag_count = 0
    ins_init()
    r = parse(content)
    encode_item(r)

def compile2(src, des = None):
    return _compile(load(src), des)

def _compile(txt, des = None):
    encode( txt )
    code = gen_code(tag_count)
    if des: save(des, code)
    return code
def compile2list(src):
    encode(load(src))
    return gen_code(tag_count, True)
    
def main( ):
    if len(ARGV) < 2:pass
    elif len(ARGV) == 2:
        compile2(ARGV[1], 'bin')
    elif len(ARGV) == 3:
        compile2(ARGV[1], ARGV[2])
if __name__ == "__main__":
    main()
