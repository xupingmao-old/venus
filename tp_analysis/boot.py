import sys

argv = sys.argv

def load( name ):
    fp = open(name, "rb")
    t = fp.read()
    fp.close()
    return t

def short( a, b):
    return (a << 8) + b
_code = ''

def code(type, val):
    _code += chr(type)
    if val == None:
        pass
    elif isinstance(val, 'name'):
        pass