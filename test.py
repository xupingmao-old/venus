from tokenize import *
from parse import *

x = 10
l = [1,2]
x = l[0]

x, y = 10, 20

x, y = [10, 20]

x = [10]

x = 10

r = tokenize(open('tokenize.py').read())

#for i in r:
#	print i.type, i.val

def show(tree):
	#print(self.tree)
	def f(n, v):
		rs = ''
		if isinstance(v, list):
			s = ''
			for i in v:
				s += f(n, i) + '\n'
			return '[\n' + s + ']'
		elif isinstance(v, AST_LIST):
			rs = 'list:\n' + f(n + 2, v.val)
		elif isinstance(v, Token):
			if v.type == 'string':
				rs = "'" + v.val + "'"
			else:rs = str(v.val)
		elif isinstance(v, Arg):
			rs = 'arg:'+v.name+',type:'+v.type+',val:'+str(v.val)
		elif isinstance(v, AST_OP):
			rs = 'op:'+ v.op + '\n' + f(n + 2 , v.left) + '\n' + f(n + 2, v.right)
		elif isinstance(v, AST_FUNC):
			rs = 'func:' + v.name + '\n' + f(n + 2, v.args) + '\n' + f(n + 2, v.body)
		elif isinstance(v, AST_RETURN):
			rs = 'return' + '\n' + f(n + 2, v.val)
		elif isinstance(v, AST_CALL):
			rs = '$func:\n' + f(n + 2, v.name) + '\n' + f(n + 2, v.args)
		elif isinstance(v, AST_FOR):
			rs = 'for\n' + f(n + 2, v.cond) + '\n' + f(n + 2, v.body)
		elif isinstance(v, AST_IF):
			rs = 'if\n' + f(n + 2, v.cond) + '\n' + f(n + 2, v._if)
			rs += '\n' + f(n + 2, v._elif)
			rs += '\n' + f(n + 2, v._else)
		elif isinstance(v, AST_ELIF):
			rs = 'elif\n' + f(n+2, v.cond) + '\n' + f(n+2, v.body)
		elif isinstance(v, AstNode):
			rs = 'type:' + v.type + '\n' + f(n+2, f.val)
		else:
			rs = str(v)
		return n * ' ' + rs
	for i in tree:
		s = f(0, i)
		print(s)

tree = parse(open('test.py').read())
show(tree)