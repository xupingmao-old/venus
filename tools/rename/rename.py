def load( name ):
    fp = open(name, "r")
    t = fp.read()
    fp.close()
    return t

def save( name, content):
    fp = open(name, 'w')
    fp.write(content)
    fp.close()


def rename(f, rp_list):
    t = load(f)
    for old,new in rp_list:
        t = t.replace(old, new)
    save(f, t)

def main(dir, rp_list):
    import os
    files = os.listdir(dir)
    for f in files:
        if os.path.isfile(f):
            rename(f, rp_list)

if __name__ == '__main__':
    main('.', ['tm_map', 'tm_dict'])
