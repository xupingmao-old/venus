class Token:
    def __init__(self, k, v):
        self.k = k
        self.v = v

    def __str__(self):
        return 'k = ' + self.k + ', v = '+ str(self.v)


class Tokenizer:

    def __init__(self):
        self.n_list = '0123456789'
        self.s_list = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$'
        self.x = 0
        self.y = 0
        self.sn_list = self.n_list + self.s_list
        self.sb = '+-*/%=:.()<>{}[]'
        self.sb_list = ['>=', '<=' , '==',
                        '>', '<' , '=',
                        '.', ':', ',',
                        '+=', '-=', '*=', '/=', '%=',
                        '+', '-', '*', '/', '%'
                        '[', ']', '{', '}', '(', ')']
        self.tk_list = []

    def load(self, src):
        self.src = src
        self.i = 0
        self.r = 0
        self.l = len(src)
        while self.parse():
            pass

    def new_tok(self, k, v, i):
        self.i = i
        self.tk_list.append(Token(k,v))

    def show(self):
        for i in self.tk_list:
            print(i)


    def do_indent(self, i):
        s = self.src
        l = self.l
        while i < l and s[i] in '\t ':
            i+=1
        if i < l and s[i] == '\n':
            while i < l and s[i] == '\n':
                i+=1
            k = 0
            while i < l and s[i] in '\t ':
                i+=1
                k+=1
            self.new_tok('indent', k, i)
            return True
        else:
            self.new_tok(':', 0, i)
            return True

    def parse(self):
        i = self.i
        l = self.l
        s = self.src
        n = self.n_list
        s_list = self.s_list
        sn_list = self.sn_list
        sb_list = self.sb_list
        if i >= l:
            self.new_tok('eof', -1, i+1)
            return False
        if s[i] in '\t ':
            while i < l and s[i] in '\t ':
                i+=1
            self.i = i
            return self.parse()
        if s[i] == '\n':
            self.new_tok('nl',None, i+ 1)
            return True
        if s[i] in n:
            while i < l and s[i] in n:
                i+=1
            if i < l and s[i] == '.':
                i+=1
                while i < l and s[i] in n:
                    i+=1
            self.new_tok('num', s[self.i:i], i)
            return True
        elif s[i] in s_list:
            while i < l and s[i] in sn_list:
                i+=1
            self.new_tok('name', s[self.i:i], i)
            return True
        elif s[i] in '+-*/%=><':
            i+=1
            if i < l and s[i] == '=':
                self.new_tok('sb', s[i-1:i],i+1)
                return True
            self.new_tok('sb', s[i-1], i)
            return True
        elif s[i] == ':':
            return self.do_indent(i+1)
        elif s[i] in '.{}()[],;':
            self.new_tok('sb', s[i], i+1)
            return True
        elif s[i] in '\'\"':
            c = s[i]
            i+=1
            while i < l:
                if s[i] == c:
                    i+=1
                    break
                if s[i] == '\\':
                    i+=1
                i+=1
            self.new_tok('str', s[self.i+1:i-1], i)
            return True
        self.new_tok('e', s[i], i+1)
        return True

t = Tokenizer()
t.load(open('tokenizer.py').read())
t.show()
