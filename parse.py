from tokenize import *

class ParserCtx:
	def __init__(self, r):
		r.append(Token('eof'))
		self.r = r
		self.i = 0
		self.l = len(r)
		self.tree = []
		self.token = Token("start")
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
	def expect(self, v):
		# print("cur=" + str(self.i))
		# print("curtoken="+ str(self.r[self.i-1].val))
		# print("p.token="+str(self.token.val))
		# print("expect="+v)
		assert self.token.val == v
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
		if v == ',':
			r = self.tree.pop()
			l = self.tree.pop()
			if isinstance(l, list):
				l.append(r)
				self.tree.append(l)
			else:
				self.tree.append([',', l, r])
		else:
			r = self.tree.pop()
			l = self.tree.pop()
			self.tree.append([v, l, r])
	def show(self):
		#print(self.tree)
		def f(v):
			if isinstance(v, list):
				s='('
				for i in v:
					s += f(i)
					s += " "
				s+=')'
				return s
			elif isinstance(v, Token):
				return str(v.val)
			else:
				return str(v)
		for i in self.tree:
			s = f(i)
			print(s)
	def showTokens(self):
		for i in self.r:
			i.show()

def parse(v):
	r = tokenize(v)
	p = ParserCtx(r)
	do_prog(p)

# recursive desent

def factor(p):
	t = p.token.type
	token = p.token
	if t in ['number', 'string', 'name']:
		p.next()
		p.add(token)
		if p.token.type == '(':
			p.next()
			if p.token.type == ')':
				p.add(None)
				p.addOp('$')
			else:
				expr(p)
				p.expect(')')
				p.addOp('$')
		elif p.token.type == '[':
			p.next()
			if p.token.type == ']':
				p.add(None)
				p.addOp('$')
			else:
				expr(p)
				p.expect(']')
				p.addOp('.')
	elif t in ['+', '-']:
		p.next()
		p.addOp(token)
	elif t == '[':
		p.next()
		expr(p)
		p.expect(']')
	elif t == '(':
		p.next()
		expr(p)
		p.expect(')')

def _expr( func, val):
	def f(p):
		func(p)
		while p.token.type == val:
			token = p.token
			p.next()
			func(p)
			p.addOp(val)
	return f
def _expr2(func, val):
	def f(p):
		func(p)
		while p.token.type in val:
			token = p.token
			p.next()
			func(p)
			p.addOp(token)
	return f
dot_call_get = _expr2(factor, '.')
item2 = _expr2(dot_call_get, ['*', '/', '%'])
item = _expr2(item2, ['+', '-'])
in_expr = _expr(item, 'in')
compare = _expr2(in_expr, ['>', '<', '>=', '<=', '==', '!='])
and_expr = _expr(compare, 'and')
or_expr = _expr(and_expr, 'or')
comma = _expr(or_expr, ',')
assign = _expr(comma, '=')

expr = assign


def do_from(p):
	p.next()
	expr(p)
	p.expect("import")
	if p.token.type == "*":
		p.add(p.token)
		p.next()
	else:
		expr(p)
	p.addOp("import")

def do_import(p):
	p.next()
	expr(p)
	p.add(None)
	p.addOp("import")

def skip_nl(p):
	p.next()
	while p.token.type == 'nl':
		p.next()


def do_block(p):
	while p.token.type != 'eof':
		skip_nl(p)
		t = p.token.type
		if t == 'from':
			do_from(p)
		elif t == 'import':
			do_import(p)
		elif t == "assert":
			do_assert(p)
		elif t == 'def':
			do_def(p)
		else:
			expr(p)

def do_assert(p):
	p.next(p)
	expr(p)
	p.add(None)
	p.addOp("assert")


def do_def(p):
	p.next(p)
	do_arg(p)
	p.expect(':')
	do_block(p)

def do_prog(p):
	#p.showTokens()
	do_block(p)
	p.show()