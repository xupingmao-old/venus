from tokenize import *
from expression import *



class ParserCtx:
    def __init__(self, r):
        r.append(Token('nl'))
        r.append(Token('eof'))
        self.r = r
        self.i = 0
        self.l = len(r)
        self.tree = []
        self.token = Token("nl")
        self.eof = Token("eof")
    def match(self, v):
        x = self.next()
        makesure (x.val == v)
    def next(self):
        if self.i < self.l:
            self.token = self.r[self.i]
            self.i+=1
        else:
            self.token = self.eof
    def back(self):
        if self.i != 0:
            self.i -= 1
    def pop(self):
        return self.tree.pop()
    def expect(self, v):
        makesure(self.token.type == v, v, self.token)
        self.next()
    def add(self, v):
        self.tree.append(v)
    def addOp(self,v):
        r = self.tree.pop()
        l = self.tree.pop()
        node = AstNode(v)
        node.a = l
        node.b = r
        self.tree.append( node )
    def enterBlock(self):
        self.tree.append('block')
        do_block(self)
        body = []
        v = self.tree.pop()
        while v != 'block':
            body.append(v)
            v = self.tree.pop()
        body.reverse() # reverse the body
        return body
    def showTokens(self):
        for i in self.r:
            i.show()
    def print_error(self):
        print(self.error())
    def error(self):
        return ' at ' + str(self.token.pos) + ' type = ' + self.token.type

def parse(v):
    r = tokenize(v)
    p = ParserCtx(r)
    x = do_prog(p)
    # except:
    if x == None:
        print(" at line " + str( p.token.pos ) + " unknown error")
    return x

def name2str(obj):
    if obj.type == 'name':
        obj.type = 'string'
    elif obj.type == ',':
        name2str(obj.a)
        name2str(obj.b)

def do_from(p):
    p.next()
    expr(p)
    p.expect("import")
    node = AstNode("from")
    node.a = p.pop()
    name2str(node.a)
    if p.token.type == "*":
        p.token.type = 'string'
        node.b = p.token
        p.next()
    else:
        expr(p)
        node.b = p.pop()
        name2str(node.b)
    p.add( node )

def do_import(p):
    p.next()
    expr(p)
    p.add(None)
    p.addOp("import")

# count = 1
skip_op = ['nl', ';']
def skip_nl(p):
    # global count
    while p.token.type in skip_op:
        p.next()
        # count+=1
        # print(count)
def call_node( fnc, args ):
    node = AstNode('$')
    node.name = fnc
    node.args = args
    return node
def do_raise(p):
    fnc = p.token
    fnc.type = 'name'
    p.next()
    if p.token.type == 'nl':
        args = None
    else:
        expr(p)
        args = p.pop()
    p.add( call_node(fnc, args))


def do_stm(p):
    t = p.token.type
    if t == 'from':
        do_from(p)
    elif t == 'import':
        do_import(p)
    elif t == 'def':
        do_def(p)
    elif t == 'class':
        do_class(p)
    elif t == 'for' or t == 'while':
        do_for_while(p, t)
    elif t == 'if':
        do_if(p)
    elif t == 'return':
        do_stm1(p, t)
    elif t == 'raise':
        do_raise(p)
    elif t == 'break' or t == 'continue' or t == 'pass':
        p.next()
        node = AstNode(t)
        p.add( node )
    elif t == 'name':
        expr(p)
    elif t == 'try':
        do_try(p)
    elif t == 'global':
        p.next()
        node = AstNode('global')
        makesure (p.token.type == 'name', p.error())
        node.val = p.token
        p.add( node )
        p.next()
    else:
        raise Exception('unknown expression'+ p.error())

def do_try(p):
    p.next()
    p.expect(':')
    node = AstNode("try")
    node.first = p.enterBlock()
    p.expect('except')
    if p.token.type == 'name':
        p.error = p.token
        p.next()
    p.expect(':')
    node.second = p.enterBlock()
    p.add( node )


def do_block(p):
    skip_nl(p)
    if p.token.type == 'indent':
        p.next()
        while p.token.type != 'dedent':
            do_stm(p)
            skip_nl(p)
        p.next()
    else:
        while p.token.type != 'nl':
            do_stm(p)
            while p.token.type == ';':
                p.next()
        skip_nl(p)
            
            

def do_if(p):
    ast = AstNode("if")
    ast.right = None
    p.next()
    expr(p)
    ast.cond = p.pop()
    p.expect(':')
    ast.left = p.enterBlock()
    cur = ast # temp
    temp = cur 
    if p.token.type == 'elif':
        while p.token.type == 'elif':
            node = AstNode("if")
            node.right = None
            p.next()
            expr(p)
            p.expect(':')
            node.cond = p.pop()
            node.left = p.enterBlock()
            cur.right = node
            cur = node
    if p.token.type == 'else':
        p.next()
        p.expect(':')
        cur.right = p.enterBlock()
    p.add(temp)


def do_for_while(p, type):
    ast = AstNode()
    ast.type = type
    p.next()
    expr(p)
    ast.a = p.pop()
    p.expect(':')
    ast.b = p.enterBlock()
    p.add(ast)

def do_arg(p):
    p.expect('(')
    if p.token.type == ')':
        p.next()
        return None
    else:
        args = []
        # []
        # [ arg * ]
        # [ arg , arg = v ]
        while p.token.type == 'name':
            arg = AstNode("arg")
            arg.val = None
            arg.name = p.token
            p.next()
            if p.token.type == '=':
                p.next()
                factor(p) # problem
                arg.val = p.pop()
            args.append(arg)
            if p.token.type != ',':break
            p.next()
        #check arguments
        arg_with_default_val = False
        for arg in args:
            if arg_with_default_val and arg.val == None:
                raise Exception('parameter without default value can not behind parameter with default value')
            if arg.val != None:
                arg_with_default_val = True
        if p.token.type == '*':
            p.next()
            makesure (p.token.type == 'name', 'invalide arguments ' + p.error())
            arg = AstNode("varg")
            arg.val = None
            arg.name = p.token
            args.append(arg)
            p.next()
        p.expect(')')
        if len(args) > 0 : return args
        return None

def do_def(p):
    p.next()
    makesure (p.token.type == 'name')
    func = AstNode("def")
    func.name = p.token
    p.next()
    func.args = do_arg(p)
    p.expect(':')
    func.body = p.enterBlock()
    p.add(func)

def do_class(p):
    p.next()
    makesure( p.token.type == 'name', 'ClassException' + p.error())
    clazz = AstNode()
    clazz.type = 'class'
    clazz.name = p.token
    p.next()
    if p.token.type == '(':
        p.next()
        makesure (p.token.type == 'name', 'ClassException'+p.error())
        p.parent = p.token.val
        p.next()
        p.expect(')')
    p.expect(':')
    clazz.body = p.enterBlock()
    p.add(clazz)

stm_end_list = ['nl', 'dedent']
def do_stm1(p, type):
    p.next()
    node = AstNode(type)
    if p.token.type in stm_end_list:
        node.val = None
    else:
        expr(p)
        node.val = p.pop()
    p.add(node)



def do_prog(p):
    p.next()
    while p.token.type != 'eof':
        do_block(p)
    return p.tree


def main():
    if len(ARGV) > 1:
        f = ARGV[1]
    else:
        print("parse.py file")
        return
    tree = parse( load(f) )
    show(tree)
    
if __name__ == "__main__":
    main()
    
