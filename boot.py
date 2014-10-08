import sys

argv = sys.argv

def load( name ):
    fp = open(name, "r")
    t = fp.read()
    fp.close()
    return t

def short( a, b):
    return (a << 8) + b
_code = ''

def code8(ins):
    return chr(ins)

def code16(ins):
    return chr(ins<<8) + chr(ins & 0xff)

def code(type, val):
    _code += chr(type)
    if val == None:
        pass
    elif isinstance(val, 'name'):
        pass