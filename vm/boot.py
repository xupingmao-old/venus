import sys
import struct
import os
ARGV = sys.argv
argv = sys.argv

def load( name ):
    fp = open(name, "rb")
    t = fp.read()
    fp.close()
    return t

def makesure( v , expect = None, cur = None):
    if not v and cur:
        print('error at ' + str( cur.pos ) + ' expect ' + expect + ' but see ' + cur.val)

def save( name, content):
    fp = open(name, 'wb')
    fp.write(content)
    fp.close()

def rm(fname):
    os.remove(fname)
def _and(a,b):
    return a and b
def _slice(self, start, end):
    return self[start:end]
    
def istype(val,  type):
    if type == 'string':
        return isinstance(val, str)
    elif type == 'number':
        return isinstance(val, int) or isinstance(val, float)
    elif type == 'list':
        return isinstance(val, list)
    elif type == 'dict':
        return isinstance(val, dict)
def typeof(val):
    if isinstance(val, str):return 'string'
    elif isinstance(val, int) or isinstance(val, float):return 'string'
    elif isinstance(val, list) or isinstance(val, tuple):return 'list'
    elif isinstance(val, dict) :return 'dict'

def short( a, b):
    return (a << 8) + b

def getshort(a, b):
    return (ord(a) << 8) + ord(b)

nextshort = getshort
_code = ''

def code8(ins):
    return chr(ins)

def code16(ins):
    return chr((ins>>8) & 0xff) + chr(ins & 0xff)

def codeF(value):
    return struct.pack('d', value)

def code(type, val):
    if istype(val, "string"):
        return chr(type) + code16(len(val))+ val
    elif istype(val, "number"):
        return chr(type) + codeF(val)
