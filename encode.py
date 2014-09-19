# encode for

from parse import *

def emit(s):
    print(s)

def store(t):
    if t.type == 'name':
        emit('store '+t.val)
    elif t.type in ['attr', 'get']:
        encode_item(t.b)
        encode_item(t.a)
        emit('SET')
    elif t.type == ',':
        store(t.b)
        store(t.a)

def store_name(t):
    emit('store ' + t)

def encode_item( tk ):
    if tk == None: return 0
    if isinstance(tk, list):
        for i in tk:
            encode_item(i)
            if i.type == '$': emit('POP')
        return
    t = tk.type
    if t == '=':
        encode_item(tk.b)
        store(tk.a)
    elif t == ',':
        return encode_item( tk.a ) + encode_item(tk.b)
    elif tk.type == 'list':
        n = encode_item(tk.val)
        emit('list '+str(n))
    elif t == '$':
        encode_item(tk.name)
        n = encode_item(tk.args)
        emit('call ' + str(n))
    elif t == 'arg':
        if tk.val:
            encode_item(tk.val)
            store(tk.name)
        #emit('set_arg '+tk.name)
    elif t == 'def':
        emit('DEF')
        encode_item(tk.args)
        emit('LOAD_PARAMS')
        encode_item(tk.body)
        emit('EOF')
        store(tk.name)
    elif t == 'return':
        if tk.val:encode_item(tk.val)
        emit('return')
    elif t == 'from':
        encode_item(Token('name','importfrom'))
        n = encode_item(tk.a)
        n += encode_item(tk.b)
        emit('call '+str(n))
        emit('POP')
    elif t in  ['+', '-', '*', '/', '%', 
                    '+=', '-=', '/=', '*=', 'get',
                    "==", "!=", ">", "<", ">=", "<=", "and", "or", "for","while", "in"]:
        encode_item(tk.a)
        encode_item(tk.b)
        emit(tk.type)
        return 1
    elif t in ['number', 'name']:
        emit('load '+tk.val)
        return 1
    elif t == 'string':
        emit('load "' + tk.val + '"')
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

main()