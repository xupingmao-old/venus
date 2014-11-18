from encode import *
from boot import *

global_mod_list = [LOAD_GLOBAL, STORE_GLOBAL, LOAD_CONSTANT, TM_DEF]

def dis( fname ,type = None):
    s = load(fname)
    i = 0; l = len(s)
    constants = [None]
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
        if ins == NEW_STRING:
            ll = nextshort(s[i], s[i+1])
            i+=2
            val = s[i:i+ll]
            i+=ll
            constants.append(val)
        elif ins == NEW_NUMBER:
            val = 0
            i+=8
            constants.append(val)
        elif ins in mode1:
            print(codes[ins])
        elif ins in mode2:
            v = ord(s[i])
            print(codes[ins]+ ' ' + str(v))
            i+=1
        elif ins in global_mod_list:
            ll = nextshort(s[i], s[i+1])
            print(codes[ins] + ' ' + str(constants[ll]))
            i+=2
        elif ins in mode3:
            ll = nextshort(s[i], s[i+1])
            print(codes[ins] + ' ' + str(ll))
            i+=2
        else:
            print("unknown instruction " + str(ins))


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
