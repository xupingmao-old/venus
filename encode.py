# encode for

from parse import *

def encode_item( tk , type = None):
    if isinstance(tk, list):
        for i in tk:
            encode_item(i)
    elif tk.type == '=':
        encode_item(tk.b)
        if tk.a.type == 'name':
            print('store '+tk.a.val)
        elif tk.a.type in ['attr','get']:
            encode_item(tk.a.a)
            encode_item(tk.a.b)
            print('set')
    elif tk.type == ',':
        encode_item(tk.a, type)
        encode_item(tk.b, type)
    elif tk.type == 'list':
        print("loadlist")
        encode_item(tk.val, 'append ')
    elif tk.type in  ['from', '+', '-', '*', '/', '%', 
                    '+=', '-=', '/=', '*=', 'get', 'attr', 
                    "==", "!=", ">", "<", ">=", "<=", "and", "or", "for","while", "in"]:
        encode_item(tk.a)
        encode_item(tk.b)
        print(tk.type)
    elif tk.type in ['number', 'string', 'name', 'star']:
        if type:
            print(type+tk.val)
        else:
            print('load ' + tk.val)
    else:
        pass


def encode(content):
    r = parse(content)
    try:encode_item(r)
    except Exception as e:print(e)

encode(open('test_op.py').read())