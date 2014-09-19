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

def load(t):
    if t.type in ['number', 'string', 'name', 'star']:
        emit('load '+t.val)
        return 1
    elif t.type == ',':
        x = load(t.a)
        return x +load(t.b)
    elif t.type == 'attr':
        load(t.a)
        load(t.b)
        emit('GET')
        return 1

def import_(des, src = None):
    load(Token('name','import'))
    n = load(des)
    if src:n += load(src)
    emit('call '+str(n))


def encode_item( tk ):
    if isinstance(tk, list):
        for i in tk:
            encode_item(i)
        return
    t = tk.type
    if tk.type == '=':
        encode_item(tk.b)
        store(tk.a)
    elif tk.type == ',':
        load(tk)
    elif tk.type == 'list':
        n = load(tk.val)
        emit('list '+str(n))
    elif t == 'arg':
        if tk.val:
            encode_item(tk.val)
            store(tk.name)
        emit('set_arg '+tk.name)
    elif t == 'def':
        emit('DEF')
        encode_item(tk.args)
        encode_item(tk.body)
        emit('EOF')
        store_name(tk.name)
    elif t == 'return':
        if tk.val:encode_item(tk.val)
        emit('return')
    elif t == 'from':
        import_(tk.b, tk.a)
    elif t in  ['+', '-', '*', '/', '%', 
                    '+=', '-=', '/=', '*=', 'get', 'attr', 
                    "==", "!=", ">", "<", ">=", "<=", "and", "or", "for","while", "in"]:
        encode_item(tk.a)
        encode_item(tk.b)
        emit(tk.type)
    elif tk.type in ['number', 'string', 'name', 'star']:
        load(tk)
    else:
        pass


def encode(content):
    r = parse(content)
    try:encode_item(r)
    except Exception as e:print(e)

encode(open('test1.py').read())