from encode import *
from boot import *

global_mod_list = [LOAD_GLOBAL, STORE_GLOBAL, LOAD_CONSTANT, TM_DEF]

def dis( fname ,type = None):
    s = load(fname)
    i = 0; l = len(s)
    constants = ['None']
    if type == 'const':
        while i < l:
            ins = s[i]
            i+=1
            ins = ord(ins)
            if ins == NEW_STRING:
                ll = nextshort(s[i], s[i+1])
                i+=2
                val = _slice(s,i,i+ll)
                i+=ll
                constants.append(val)
            elif ins == NEW_NUMBER:
                val = unpack('f', _slice(s,i,i+8))
                i+=8
                constants.append(val)
            else:
                break
        for v in constants:
            print(v)
        return
    while i < l:
        ins = s[i]
        i+=1
        ins = ord(ins)
        val = nextshort(s[i], s[i+1])
        i+=2
        if ins == NEW_STRING or ins == NEW_NUMBER:
            vv = s[i:i+val]
            i+=val
            constants.append(vv)
            print(codes[ins] + ' ' + str(vv))
        elif ins == LOAD_CONSTANT or ins == LOAD_GLOBAL or ins == STORE_GLOBAL:
            print(codes[ins] + ' ' + constants[val])
        else:
            print(codes[ins] + ' ' + str(val))


def main():
    if len( ARGV ) == 2:
        fname = ARGV[1]
        dis(fname)
    elif len(ARGV) == 3 and ARGV[1] == 'src':
        fname = ARGV[2]
        tmp = save_bin(fname, "temp")
        dis("temp")
        rm("temp")
    elif len(ARGV) == 3 and ARGV[1] == 'const':
        fname = ARGV[2]
        dis(fname, 'const')

if __name__ == "__main__":
    main()
