from tokenize import *


class AstNode:
	def __init__(self):
		self.type = None

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
		assert self.token.val == v, 'error at ' + str(self.token.pos)
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
			if isinstance(l, AST_LIST):
				l.val.append(r)
				self.tree.append(l)
			else:
				self.tree.append(AST_LIST([l,r]))
		else:
			r = self.tree.pop()
			l = self.tree.pop()
			self.tree.append(AST_OP(v, l, r))
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
	def enterBlock(self, func):
		self.tree.append('block')
		func(self)
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

def parse(v):
	r = tokenize(v)
	p = ParserCtx(r)
	return do_prog(p)

# recursive desent

def factor(p):
	t = p.token.type
	token = p.token
	if t in ['number', 'string', 'name']:
		p.next()
		p.add(token)
	elif t in ['+', '-']:
		p.next()
		factor(p)
		node = AstNode()
		node.type = t
		node.val = p.pop()
		p.add( node )
	elif t == '[':
		p.next()
		if p.token.type == ']':
			p.next()
			p.add(AST_LIST(None))
		else:
			expr(p)
			p.expect(']')
			p.add( AST_LIST( p.pop()))
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
			p.addOp(token.type)
	return f
def _expr2(func, val):
	def f(p):
		func(p)
		while p.token.type in val:
			token = p.token
			p.next()
			func(p)
			p.addOp(token.type)
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
				p.addOp('.')
			elif t == '(':
				if p.token.type == ')':
					p.next()
					p.add( AST_CALL(p.pop(), None))
				else:
					expr(p)
					p.expect(')')
					args = p.pop()
					p.add( AST_CALL(p.pop(), args))
			else:
				func(p)
				p.addOp('.')
	return f

dot_expr = _expr4(factor)
item2 = _expr2(dot_expr, ['*', '/', '%'])
item = _expr2(item2, ['+', '-'])
in_expr = _expr(item, 'in')
compare = _expr2(in_expr, ['>', '<', '>=', '<=', '==', '!='])
and_expr = _expr(compare, 'and')
or_expr = _expr(and_expr, 'or')
comma = _expr(or_expr, ',')
assign = _expr2(comma, ['=', '+=', '-=', '*=', '/='])

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
	while p.token.type == 'nl':
		p.next()

def do_raise(p):
	p.next()
	if p.tolen.type == 'nl':
		p.add(['raise', None])
	else:
		expr(p)
		p.addOp2('raise')

def do_stm(p):
	while p.token.type == 'nl':
		p.next()
	t = p.token.type
	if t == 'from':
		do_from(p)
	elif t == 'import':
		do_import(p)
	elif t == "assert":
		do_assert(p)
	elif t == 'def':
		do_def(p)
	elif t == 'for':
		do_for(p)
	elif t == 'return':
		do_return(p)
	elif t == 'raise':
		do_raise(p)
	elif t == 'pass':
		p.next()
		p.add('pass')
	elif t == 'if':
		do_if(p)
	elif t == 'name':
		expr(p)
	else:
		raise Exception('unknown expression, type = ' + t 
			+ ', pos = ' + str(p.token.pos))

def do_block(p):
	skip_nl(p)
	if p.token.type == 'indent':
		p.next()
		while p.token.type != 'dedent':
			do_stm(p)
			skip_nl(p)
		p.next()
	else:
		do_stm(p)

def do_if(p):
	ast = AST_IF()
	p.next()
	expr(p)
	ast.cond = p.pop()
	p.expect(':')
	ast._if = p.enterBlock(do_block)
	if p.token.type == 'elif':
		body = []
		while p.token.type == 'elif':
			p.next()
			expr(p)
			p.expect(':')
			cond = p.pop()
			b = p.enterBlock(do_block)
			body.append( AST_ELIF(cond, b) )
		ast._elif = body
	if p.token.type == 'else':
		p.next()
		p.expect(':')
		ast._else = p.enterBlock(do_block)
	p.add(ast)


def do_for(p):
	ast = AST_FOR()
	p.next()
	expr(p)
	ast.cond = p.pop()
	p.expect(':')
	ast.body = p.enterBlock(do_block)
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
		while p.token.type in ['*', 'name']:
			arg = Arg()
			if p.token.type == '*':
				p.next()
				arg.type = '*'
			else:
				arg.type = 'normal'
			assert p.token.type == 'name'
			arg.name = p.token.val
			p.next()
			if p.token.type == '=':
				p.next()
				factor(p)
				arg.val = p.pop()
			args.append(arg)
			if p.token.type != ',':
				break
			p.next()
		#check sequence
		if len(args) > 0:
			state = 1
			# 1 : normal no default value
			# 2 : normal with default value
			# 3 : *
			for arg in args:
				if state == 2:
					assert arg.val != None, 'invalid arguments, error at ' + str(p.token.pos)
				elif state == '*':
					raise
				if arg.type == '*':state = 3
				elif arg.val != None:state = 2
		p.expect(')')
		return args

def do_def(p):
	p.next()
	assert p.token.type == 'name'
	func = AST_FUNC()
	func.name = p.token.val
	p.next()
	func.args = do_arg(p)
	p.expect(':')
	func.body = p.enterBlock(do_block)
	p.add(func)

def do_return(p):
	p.next()
	if p.token.type in ['nl', 'dedent']:
		p.add( AST_RETURN (None))
	else:
		expr(p)
		p.add(AST_RETURN(p.pop()))

def do_prog(p):
	#p.showTokens()
	p.next()
	while p.token.type != 'eof':
		do_block(p)
	return p.tree
