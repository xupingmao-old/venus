import sys
import struct
argv = sys.argv

def load( name ):
    fp = open(name, "r")
    t = fp.read()
    fp.close()
    return t

def save( name, content):
    fp = open(name, 'w')
    fp.write(content)
    fp.close()

def istype(val,  type):
    if type == 'string':
        return isinstance(val, str)
    elif type == 'number':
        return isinstance(val, int) or isinstance(val, float)
    elif type == 'list':
        return isinstance(val, list)
    elif type == 'dict':
        return isinstance(val, dict)

def short( a, b):
    return (a << 8) + b
_code = ''

def code8(ins):
    return chr(ins)

def code16(ins):
    return chr((ins>>8) & 0xff) + chr(ins & 0xff)

def code(type, val):
    if istype(val, "string"):
        return chr(type) + code16(len(val))+ val
    elif istype(val, "number"):
        return chr(type) + struct.pack("d", val)
