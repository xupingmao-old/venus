#coding:utf-8
# 读取系统hosts设置

def load(path):
    try:
        txt = open(path, encoding = 'utf-8').read()
    except:
        txt = open(path).read()
    return txt

class ConfNode:
    def __init__(self, ip, addr, desc = None):
        self.ip = ip
        self.addr = addr
        self.desc = desc

    def __str__(self):
        if self.desc == None: self.desc = 'None'
        return self.addr + ' : ' + self.ip + ' , ' + self.desc

class HostConf:
    def __init__(self, path):
        self.path = path
        content = load(path)
        content = content.replace('\r', '')
        content = content.replace('\t', ' ')
        self.content = content
        lines = content.split('\n')
        self.lines = lines
        self.parse()
    def enable( self, pos):
        if pos >= len(self.frozen) or pos < 0:
            return
        node = self.frozen[pos]
        self.active.append(node)
        del self.frozen[pos]
    def disable( self, pos):
        if pos >= len(self.active) or pos < 0:
            return
        node = self.active[pos]
        self.frozen.append(node)
        del self.active[pos]
    def show(self, type='active'):
        if type == 'active':
            print("=======active host configure========")
            for i,value in enumerate(self.active):
                print(i,str(value))
        elif type == 'frozen':
            print('=======frozen host configure========')
            for i,value in enumerate( self.frozen):
                print(i,str( value ))
        else:
            print(self.frozen)
            print(self.active)
    @staticmethod
    def parse_line(line):
        wordlist = []
        word = ''
        for c in line:
            # notice the sequence of the word
            if c == '#':
                if len(word) > 0:
                    wordlist.append(word)
                wordlist.append(c)
                word = ''
            elif c != ' ' and c != '\t':
                word+=c
            elif len(word) > 0:
                wordlist.append(word)
                word = ''
        # notice here, remember to handler tails
        if len(word) > 0:
            wordlist.append(word)
        return wordlist
    def parse(self):
        active = []
        self.frozen = []
        for line in self.lines:
            wordlist = self.parse_line(line)
            if len(wordlist) < 2:
                continue
            ip, addr = wordlist[0], wordlist[1]
            desc = None
            # host configure was blocked, it may like:
            # # {ip} {addr} # {comment}*
            if ip == '#':
                ip = wordlist[1]
                if len(wordlist) >= 5 and wordlist[3] == '#':
                    addr = wordlist[2]
                    desc = wordlist[4]
                elif len( wordlist) >= 3:
                    addr = wordlist[2]
                else:
                    continue
                self.frozen.append( ConfNode(ip, addr, desc) )
            else:
                if len( wordlist) >= 4 and wordlist[2] == '#':
                    desc = wordlist[3]
                active.append( ConfNode(ip, addr, desc ) )
        self.active = active
        

conf = HostConf(open('location.ini').read())
#print(conf.tokens)
conf.show('active')
conf.show('frozen')
input('hold')
