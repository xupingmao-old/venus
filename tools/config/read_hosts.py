#coding:utf-8
# 读取系统hosts设置

class HostConf:
    def __init__(self, path):
        self.path = path
        content = open(path, encoding = "utf-8" ).read()
        content = content.replace('\r', '')
        content = content.replace('\t', ' ')
        self.content = content
        lines = content.split('\n')
        self.lines = lines
        # frozen = {}
        # active = {}
        # for line in lines:
        #     line = line.lstrip()
        #     if len(line) == 0:continue
        #     elif line.startswith('#'):
        #         line = line[1:]
        #         vv = line.split(' ')
        #         k, v = vv[0], vv[1]
        #         frozen[k] = v
        #     else:
        #         vv = line.split(' ')
        #         k, v = vv[0], vv[1]
        #         active[k] = v
        # self.frozen = frozen
        # self.active = active
        self.parse()
    def show(self):
        print(self.frozen)
        print(self.active)
    def parse(self):
        tokens = []
        s = ''
        for c in self.content:
            if c.isalnum() or c == '.' or c == '-':
                s+=c
            elif c == ' ':
                if s != '' :tokens.append(s)
                s = ''
            else:
                if s != '': tokens.append(s)
                tokens.append(c)
                s = ''
        self.tokens = tokens
        key, val = None, None
        comment = ""
        comments = []
        active = {}
        state = -2
        for t in tokens:
            if t == '#':
                comment = ""
                state = 3
            elif t == '\n':
                if state == 2:
                    active[key] = val
                elif state == -1:
                    comments.append(comment)
                state = 0
            else:
                state += 1
            if state == 1:
                val = t
            elif state == 2:
                key = t
            elif state > 2:
                comment+=t
        self.active = active
def print_effective_lines( lines ):
    print('\n\neffective configures:')
    for line in lines:
        line = line.lstrip('\r\n ')
        if line.startswith('#'):
            continue
        elif len(line) > 0:
            print(line)

def print_frozen_lines( lines):
    print('\n\nfrozen configures:')
    for line in lines:
        line = line.lstrip()
        if len(line) > 0 and line.startswith("#"):
            print(line)


def active_cainiaowork( lines ):
    pass

def frozen_cainiaowork( lines ):
    pass

def read_content():
    fname = open('location.ini').read()
    return open(fname, encoding='utf-8').read()
def main():
    txt = read_content()
    txt = txt.replace('\r', '')
    lines = txt.split('\n')
    x = input('''
your choice?
1. effective configures
2. frozen configures
3. active cainiaowork localhost
4. frozen cainiaowork localhost
5. exit
''')
    x = int(x)
    if x == 1:
        print_effective_lines(lines)
    elif x == 2:
        print_frozen_lines(lines)
    elif x == 3:
        active_cainiaowork(lines)
    elif x == 4:
        frozen_cainiaowork(lines)
    elif x == 5:
        exit(0)
    input('press enter to quit')

conf = HostConf(open('location.ini').read())
print(conf.tokens)
print(conf.active)
if __name__ == '__main__':
    main()
