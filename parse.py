from tokenize import *


class AstNode:
	def __init__(self):
		self.type = None
	def list(self, left, right):
		self.val = [left, right]

class AST_IF:
	def __init__(self):
		self._if = None
		self._elif = None
		self._else = None

class AST_ELIF:
	def __init__(self, cond, body):
		self.cond = cond
		self.body = body

class AST_FOR:
	def __init__(self):
		pass

class AST_OP:
	def __init__(self, op, left, right):
		self.op = op
		self.left = left
		self.right = right

class AST_FUNC:
	def __init__(self):
		pass
class AST_RETURN:
	def __init__(self, v):
		self.val = v
class AST_LIST:
	def __init__(self, v):
		self.val = v
	def list(self, left, right):
		self.val = [left, right]

class AST_CALL:
	def __init__(self, v, args):
		self.name = v
		self.args = args

class AST_PRE:
	def __init__(self, t, v):
		self.op = t
		self.val = v
class Arg:
	def __init__(self):
		self.val = None


class ParserCtx:
	def __init__(self, r):
		r.append(Token('eof'))
		self.r = r
		self.i = 0
		self.l = len(r)
		self.tree = []
		self.token = Token("nl")
		self.eof = Token("eof")
	def match(self, v):
		x = self.next()
		assert x.val == v
	def next(self):
		if self.i < self.l:
			self.token = self.r[self.i]
			self.i+=1
		else:
			self.token = self.eof
	def nextis(self, v):
		if self.i < self.l:
			return self.r[self.i].val == v
		return False
	def back(self):
		if self.i != 0:
			self.i -= 1
	def pop(self):
		return self.tree.pop()
	def expect(self, v):
		# print("cur=" + str(self.i))
		# print("curtoken="+ str(self.r[self.i-1].val))
		# print("p.token="+str(self.token.val))
		# print("expect="+v)
		assert self.token.type == v, 'error at ' + str(self.token.pos)
		self.next()
	def nextName(self):
		self.next()
		assert self.token.type == 'name'
	def nextSymbol(self):
		self.next()
		assert self.token.type == 'symbol'
	def add(self, v):
		self.tree.append(v)
	def addOp(self,v):
		r = self.tree.pop()
		l = self.tree.pop()
		node = AstNode()
		node.type = v
		node.a = l
		node.b = r
		self.tree.append( node )
	def addOp2(self, t):
		v = self.tree.pop()
		self.tree.append([t, v, None])
	def addOp1(self, t):
		self.tree.append([t, None])
	def addList(self):
		v = self.tree.pop()
		if isinstance(v, list):
			if v[0] == ',':
				v[0] = 'list'
			self.tree.append(v)
		else:
			self.tree.append(['list', v])
	def setArgs(self, n):
		if n == 0:
			self.tree.append(['setArgs', None])
			return
		args = []
		while n > 0:
			v = self.tree.pop()
			args.append(v)
			n-=1
		self.tree.append(['setArgs', args])
	def makeFunc(self, name, args):
		body = []
		v = self.tree.pop()
		while v != 'func_start':
			body.append(v)
			v = self.tree.pop()
		self.tree.append(['def', name, args, body])
	def setReturn(self, return_val = None):
		if not return_val:
			self.tree.append(['return', None])
		else:
			self.tree.append(['return', self.tree.pop()])
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
	def error(self):
		return ' at ' + str(self.token.pos) + ' type = ' + self.token.type

def parse(v):
	r = tokenize(v)
	p = ParserCtx(r)
	return do_prog(p)

# recursive desent

def factor(p):
	if p.token.type in ['+', '-', 'not']:
		t = p.token.type
		p.next()
		if t == '+':t='pos'
		elif t == '-':t='neg'
		factor(p)
		node = AstNode()
		node.type = t
		node.val = p.pop()
		p.add(node)
	else:
		factor_(p)

def factor_(p):
	t = p.token.type
	token = p.token
	if t in ['number', 'string', 'name', 'constants']:
		p.next()
		p.add(token)
		factor_next_if(p)
	elif t == '[':
		p.next()
		node = AstNode()
		node.type = 'list'
		if p.token.type == ']':
			p.next()
			node.val = None
		else:
			expr(p)
			p.expect(']')
			node.val = p.pop()
		p.add( node )
	elif t == '(':
		p.next()
		node = AstNode()
		node.type = 'list'
		if p.token.type == ')':
			p.next()
			node.val = None
		else:
			expr(p)
			p.expect(')')
			node.val = p.pop()
		p.add(node)

def _expr( func, val):
	def f(p):
		func(p)
		while p.token.type == val:
			t = p.token.type
			p.next()
			func(p)
			p.addOp(t)
	return f
def _expr2(func, val):
	def f(p):
		func(p)
		while p.token.type in val:
			t = p.token.type
			p.next()
			func(p)
			p.addOp(t)
	return f


def _expr4(func):
	def f(p):
		func(p)
		while p.token.type in ['.', '[', '(']:
			t = p.token.type
			p.next()
			if t == '[':
				expr(p)
				p.expect(']')
				p.addOp('get')
			elif t == '(':
				node = AstNode()
				node.type = '$'
				node.name = p.pop()
				if p.token.type == ')':
					p.next()
					node.args = None
					p.add( node )
				else:
					expr(p)
					p.expect(')')
					node.args = p.pop()
					p.add( node )
			else:
				func(p)
				p.addOp( 'attr' )
	return f

dot_expr = _expr4(factor)
item2 = _expr2(dot_expr, ['*', '/', '%'])
item = _expr2(item2, ['+', '-'])
in_expr = _expr2(item, ['in', 'notin'])
compare = _expr2(in_expr, ['>', '<', '>=', '<=', '==', '!=', 'is', 'isnot'])
and_expr = _expr(compare, 'and')
or_expr = _expr(and_expr, 'or')
comma = _expr(or_expr, ',')
assign = _expr2(comma, ['=', '+=', '-=', '*=', '/='])

expr = assign


def do_from(p):
	p.next()
	expr(p)
	p.expect("import")
	node = AstNode()
	node.type='from'
	node.a = p.pop()
	if p.token.type == "*":
		node.b = '*'
		p.next()
	else:
		expr(p)
		node.b = p.pop()
	p.add( node )

def do_import(p):
	p.next()
	expr(p)
	p.add(None)
	p.addOp("import")

def skip_nl(p):
	while p.token.type in ('nl', ';'):
		p.next()

def do_raise(p):
	p.next()
	if p.tolen.type == 'nl':
		p.add(['raise', None])
	else:
		expr(p)
		p.addOp2('raise')

def factor_next_if(p):
	if p.token.type == 'if':
		p.next()
		node = AstNode()
		node.type = 'choose'
		expr(p)
		node.cond = p.pop()
		node.body = p.pop()
		p.expect('else')
		expr(p)
		node._else = p.pop()
		p.add(node)

def do_stm(p):
	t = p.token.type
	if t == 'from':
		do_from(p)
	elif t == 'import':
		do_import(p)
	elif t == "assert":
		do_assert(p)
	elif t == 'def':
		do_def(p)
	elif t == 'class':
		do_class(p)
	elif t in ('for', 'while'):
		do_for_while(p, t)
	elif t == 'if':
		do_if(p)
	elif t in ('return', "raise"):
		do_stm1(p, t)
		#stm_next_if(p)
	elif t in ("break", "continue", "pass"):
		p.next()
		node = AstNode()
		node.type = "pass"
		p.add( node )
	elif t == 'name':
		expr(p)
		#stm_next_if(p)
	elif t == 'try':
		do_try(p)
	elif t == 'global':
		p.next()
		node = AstNode()
		node.type = 'global'
		assert p.token.type == 'name', p.error()
		p.next()
		node.val = p.token.val
		p.add( node )
	else:
		raise Exception('unknown expression'+ p.error())
	skip_nl(p)

def do_try(p):
	p.next()
	p.expect(':')
	node = AstNode()
	node.type = 'try'
	node.body = p.enterBlock()
	p.expect('except')
	if p.token.type == 'name':
		p.error = p.token
		p.next()
	p.expect(':')
	node.catch = p.enterBlock()
	p.add( node )


def do_block(p):
	skip_nl(p)
	if p.token.type == 'indent':
		p.next()
		while p.token.type != 'dedent':
			do_stm(p)
		p.next()
	else:
		do_stm(p)
			

def do_if(p):
	ast = AstNode()
	ast.type = 'if'
	ast._else = None
	p.next()
	expr(p)
	ast.cond = p.pop()
	p.expect(':')
	ast.body = p.enterBlock()
	temp = cur = ast # temp 
	if p.token.type == 'elif':
		while p.token.type == 'elif':
			node = AstNode()
			node.type , node._else = 'if', None
			p.next()
			expr(p)
			p.expect(':')
			node.cond = p.pop()
			node.body = p.enterBlock()
			cur._else = node
			cur = node
	if p.token.type == 'else':
		p.next()
		p.expect(':')
		cur._else = p.enterBlock()
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

def do_assert(p):
	p.next()
	expr(p)
	p.add(None)
	p.addOp("assert")

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
			arg = AstNode()
			arg.type = 'arg'
			arg.val = None
			arg.name = p.token.val
			p.next()
			if p.token.type == '=':
				p.next()
				factor(p) # problem
				arg.val = p.pop()
			args.append(arg)
			if p.token.type != ',':break
			p.next()
		if p.token.type == '*':
			p.next()
			assert p.token.type == 'name', 'invalide arguments ' + p.error()
			arg = AstNode()
			arg.type = 'varg'
			arg.val = None
			arg.name = p.token.val
			args.append(arg)
			p.next()
		p.expect(')')
		return args if len(args) > 0 else None

def do_def(p):
	p.next()
	assert p.token.type == 'name'
	func = AstNode()
	func.type = 'def'
	func.name = p.token.val
	p.next()
	func.args = do_arg(p)
	p.expect(':')
	func.body = p.enterBlock()
	p.add(func)

def do_class(p):
	p.next()
	assert p.token.type == 'name', 'ClassException' + p.error()
	clazz = AstNode()
	clazz.type = 'class'
	clazz.name = p.token.val
	p.next()
	if p.token.type == '(':
		p.next()
		assert p.token.type == 'name', 'ClassException'+p.error()
		p.parent = p.token.val
		p.next()
		p.expect(')')
	p.expect(':')
	clazz.body = p.enterBlock()
	p.add(clazz)

def do_stm1(p, type):
	p.next()
	node = AstNode()
	node.type = type
	if p.token.type in ['nl', 'dedent']:
		node.val = None
	else:
		expr(p)
		node.val = p.pop()
	p.add(node)

def do_prog(p):
	#p.showTokens()
	p.next()
	while p.token.type != 'eof':
		do_block(p)
	return p.tree
