
class Token:
    pass

class Parser:

    def load(self, content):
        self.content = content
        self.len = len(content)
        self.i = 0
        self.x = 0
        self.y = 0
        self.tokens=[]

    def add(self,k,v):
        t = Token()
        t.key = k
        t.val = v
        self.tokens.append(t)

    def parse(self):
        k = self.do_name()
        self.expect('=')
        v = self.do_name()
        

    def skip_lines(self):
        s = self.content
        l = self.len
        i = self.i
        while i < l and (s[i] == ' ' or s[i] == '\n'):
            is s[i] == '\n': self.y += 1
            i+=1
        self.i = i
    # parse the file
    def do_name(self):
        self.skip_lines()
        s = self.content
        l = self.len
        i = self.i
        def is_char(s):
            return s >= 'a' and s <='z' or s >='A' and s <='Z' or s == '_' or s == '.'
        k = ''
        while is_char(s[i]):
            k, i = k+s[i], i+1
        return k
    def expect(self, v):
        self.skip_lines(self)
        assert self.content[self.i] == v , 'expect '+v
        
            
