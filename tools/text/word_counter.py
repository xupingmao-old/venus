def load(fname):
    fp = open(fname,'r')
    t = fp.read()
    fp.close()
    return t

def save(fname,txt):
    fp = open(fname,'w')
    fp.write(txt)
    fp.close()


def count(src, des):
    txt = load(src)
    s=''
    words = {}
    for c in txt:
        if c.isalpha():
            s+=c
        elif len(s) > 0:
            s = s.lower()
            if s in words:
                words[s]+=1
            else:
                words[s] = 1
            s=''
        else:
            s=''
    nw = {}
    for k in words:
        v = words[k]
        if v in nw:
            nw[v] += [k]
        else:
            nw[v] = [k]
    dest = ''
    r = [(k, nw[k]) for k in sorted(nw.keys())]
    for i in r:
        dest+=str(i)+'\n'
    save(des, dest)
    

def main():
    import sys
    argv = sys.argv
    src = 'test.html'
    des = 'result.txt'
    count(src, des)

if __name__ == '__main__':
    main()
