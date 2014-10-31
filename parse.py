from tokenize import *

class AstNode:
	def __init__(self, type=None):
		self.type = type
	def list(self, left, right):
		self.val = [left, right]

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
		makesure (x.val == v)
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
		makesure(self.token.type == v, 'error at ' + str(self.token.pos) + " expect " +str(v) +" but see "+str(self.token.val))
		self.next()
	def nextName(self):
		self.next()
		makesure (self.token.type == 'name')
	def nextSymbol(self):
		self.next()
		makesure (self.token.type == 'symbol')
	def add(self, v):
		self.tree.append(v)
	def addOp(self,v):
		r = self.tree.pop()
		l = self.tree.pop()
		node = AstNode(v)
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
	def print_error(self):
		print(self.error())
	def error(self):
		return ' at ' + str(self.token.pos) + ' type = ' + self.token.type

def parse(v):
	# try:
	r = tokenize(v)
	p = ParserCtx(r)
	x = do_prog(p)
	# except:
	if x == None:
		print(" at line " + str( p.token.pos ) + " unknown error")
	return x

# recursive desent

#def factor(p):
#	if p.token.type in ['+', '-', 'not']:
#		t = p.token.type
#		p.next()
#		if t == '+':
#			t='pos'
#		elif t == '-':
#			t='neg'
#		factor(p)
#		node = AstNode()
#		node.type = t
#		node.val = p.pop()
#		p.add(node)
#	else:
#		factor_(p)

def factor(p):
	t = p.token.type
	token = p.token
	if t in ['number', 'string', 'name', 'None']:
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
		expr(p)
		p.expect(')')
	elif t == '{':
		p.next()
		node = AstNode('dict')
		if p.token.type == '}':
			p.next()
			node.items = []
			p.add(node)
		else:
			items = {}
			while p.token.type != '}':
				factor(p)
				p.expect(':')
				factor(p)
				v = p.pop()
				k = p.pop()
				items[k] = v
				if p.token.type == '}':
					break
				p.expect(',')
			p.expect('}')
			node.items = items
			p.add(node)



# def _expr2(func, val):
# 	def f(p):
# 		func(p)
# 		while p.token.type in val:
# 			t = p.token.type
# 			p.next()
# 			func(p)
# 			p.addOp(t)
# 	return f

class MyExpr:
	def __init__(self, fnc, range):
		self.fnc = fnc
		self.range = range

	def run(self, p):
		range = self.range
		self.fnc(p)
		while p.token.type in range:
			t = p.token.type
			# print(p.token.pos)
			p.next()
			self.fnc(p)
			p.addOp(t)

class Temp:
	def __init__(self,fnc, range):
		self.fnc = fnc
		self.range = range
	def run(self, p):
		if p.token.type == '-':
			p.next()
			self.run(p)
			node = AstNode("neg")
			node.val = p.pop()
			p.add(node)
			return
		self.fnc(p)
		while p.token.type in self.range:
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
            # handler dot (.)
			else:
				self.fnc(p)
				b = p.pop()
				b.type = 'string'
				a = p.pop()
				node = AstNode('get')
				node.a = a
				node.b = b
				p.add(node)
class PreExpr:
	def __init__(self, fnc, range):
		self.fnc = fnc
		self.range = range
	def run(self, p):
		if p.token.type in self.range:
			# print(p.token.pos)
			p.next()
			self.run(p)
			node = AstNode("not")
			node.val = p.pop()
			p.add(node)
		else:
			self.fnc(p)

dot_expr = Temp(factor, ['.', '[', '('])
item2 = MyExpr(dot_expr.run, ['*', '/', '%'])
item = MyExpr(item2.run, ['+', '-'])
in_expr = MyExpr(item.run, ['in', 'notin'] )
compare = MyExpr(in_expr.run,  ['>', '<', '>=', '<=', '==', '!=', 'is', 'isnot'])
pre_expr = PreExpr(compare.run, ['not']);
and_expr = MyExpr(pre_expr.run, ['and'])
or_expr = MyExpr(and_expr.run, ['or'])
comma = MyExpr(or_expr.run, [','])
assign = MyExpr(comma.run, ['=', '+=', '-=', '*=', '/=', '%='])

# dot_expr = _expr4(factor)
# item2 = _expr2(dot_expr, ['*', '/', '%'])
# item = _expr2(item2, ['+', '-'])
# in_expr = _expr2(item, ['in', 'notin'])
# compare = _expr2(in_expr, ['>', '<', '>=', '<=', '==', '!=', 'is', 'isnot'])
# and_expr = _expr2(compare, ['and'])
# or_expr = _expr2(and_expr, ['or'])
# comma = _expr2(or_expr, [','])
# assign = _expr2(comma, ['=', '+=', '-=', '*=', '/='])

expr = assign.run

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
def skip_nl(p):
	# global count
	while p.token.type in ['nl', ';']:
		p.next()
		# count+=1
		# print(count)

def do_raise(p):
	p.next()
	node = AstNode("raise")
	if p.tolen.type == 'nl':
		node.val = None
	else:
		expr(p)
		node.val = p.pop()
	p.add(node)

def factor_next_if(p):
	if p.token.type == 'if':
		p.next()
		node = AstNode()
		node.type = 'choose'
		expr(p)
		node.cond = p.pop()
		node.left = p.pop()
		p.expect('else')
		expr(p)
		node.right = p.pop()
		p.add(node)

def do_stm(p):
	t = p.token.type
	if t == 'from':
		do_from(p)
	elif t == 'import':
		do_import(p)
	# elif t == "makesure":
	# 	do_makesure(p)
	elif t == 'def':
		do_def(p)
	elif t == 'class':
		do_class(p)
	elif t in ['for', 'while']:
		do_for_while(p, t)
	elif t == 'if':
		do_if(p)
	elif t in ['return', "raise"]:
		do_stm1(p, t)
		#stm_next_if(p)
	elif t in ["break", "continue", "pass"]:
		p.next()
		node = AstNode(t)
		p.add( node )
	elif t == 'name':
		expr(p)
		#stm_next_if(p)
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
	skip_nl(p)

def do_try(p):
	p.next()
	p.expect(':')
	node = AstNode("try")
	node.body = p.enterBlock()
	p.expect('except')
	if p.token.type == 'name':
		p.error = p.token
		p.next()
	p.expect(':')
	node.handler = p.enterBlock()
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

def do_makesure(p):
	p.next()
	expr(p)
	p.add(None)
	p.addOp("makesure")

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
	p.next()
	while p.token.type != 'eof':
		do_block(p)
	return p.tree



# def do_prog1(p):
# 	#p.showTokens()
# 	try:
# 		return compile_prog(p)
# 	except e
# 		print(e)
# 		p.error()


def sp_str(v):
	v = str(v)
	if v.find('\r') == -1 and v.find('\n') == -1:
		return v
	x = ''
	for i in v:
		if i == '\r':x+='\\r'
		elif i == '\n':x+='\\n'
		elif i == '\0':x+='\\0'
		else:x+=i
	return x


ops_list = [
		'from', '+', '-', '*', '/', '%', ',' ,'=', 
		'+=', '-=', '/=', '*=', 'get',
		"==", "!=", ">", "<", ">=", "<=", "and",
		 "or", "for","while", "in", "notin"]

cond_list = ["if", "choose"]
pre_list = ['neg', 'pos', 'not', 'list']

def f(n, v, pre = ""):
	rs = ''
	if v == None:
		rs = 'None'
	elif istype(v, "list"):
		s = ''
		for i in v:
			s += '\n' + f(n+2, i)
		rs = s
	elif v.type == 'string':
		rs = "'" + sp_str(v.val) + "'"
	elif v.type == 'number':
		rs = sp_str(v.val)
	elif v.type == 'name':
		rs = sp_str(v.val)
	elif v.type in pre_list:
		rs = v.type + '\n' + f(n+2, v.val)
	elif v.type in ops_list:
		rs = v.type + '\n' + f(n+2, v.a) + '\n' + f(n+2, v.b)
	elif v.type == '$':
		rs = 'invoke\n' + f(n+2, v.name) + '\n' + f(n+2, v.args)
	elif v.type in cond_list:
		rs = v.type+'\n' + f(n+2, v.cond, 'cond => ') + \
			'\n' + f(n+2, v.left, 'body => ') + '\n' + f(n+2, v.right, 'else => ')
	elif v.type == 'def':
		rs = 'def\n' + f(n + 2 , v.name , 'name => ') + \
			'\n' + f(n+2, v.args, 'args => ') + '\n' + f(n+2, v.body, "body => ")
	elif v.type == 'class':
		rs = 'class ' + f( 0, v.name, 'name => ') + \
			'\n' + f(n+2, v.body, 'body => ')
	elif v.type in ['varg', 'arg']:
		rs = v.type + f(1 , v.name, 'name => ') + '\n'+ f(n + 2,  v.val, 'dafault => ')
	elif v.type in ['return', 'global', 'raise']:
		rs = v.type + '\n' + f(n+2, v.val)
	elif v.type in ["break", "continue", "pass"]:
		rs = v.type
	elif v.type == 'dict':
		items = v.items
		ss = ""
		if items != None:
			for k in items:
				ss += f(n+2, k)
				ss += f(1, items[k])+'\n'
		rs = v.type + '\n'+ ss
	else:
		# print(str(type(v))+":"+str(v))
		rs = sp_str(v)
#		else:
#			rs = sp_str(v)
	return ' ' * n + pre + rs
def show(tree):
	if not istype(tree, 'list'):
		print("parameter is not a list")
		print(tree)
		return
	#print(self.tree)

	for i in tree:
		s = f(0, i)
		print(s)
# tree = parse(open('parse.py').read())

def simple_show(tree):
	if not istype(tree, "list"):
		print(tree)
	else:
		for i in tree:
			simple_show(i)

def _parse(f):
	tree = parse(load(f))
	show(tree)
def main():
	if len(ARGV) > 1:
		f = ARGV[1]
	else:
		f = 'parse.py'
	tree = parse( load(f) )
	show(tree)

if __name__ == "__main__":
	main()
