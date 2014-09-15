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
		if isinstance(v, list):
			s = ''
			for i in v:
				s += f(0, i) + ' '
			return n * ' ' +  s
		elif isinstance(v, Token):
			return n * ' ' + str(v.val)
		elif isinstance(v, Arg):
			return ' ' * n + 'arg:'+v.name+',type:'+v.type+',val:'+str(v.val)
		elif isinstance(v, AST_OP):
			return ' ' * n + 'op:'+ v.op + '\n' + f(n + 2 , v.left) + '\n' + f(n + 2, v.right)
		elif isinstance(v, AST_FUNC):
			return ' ' * n + 'func:' + v.name + '\n' + f(n + 2, v.args) + '\n' + f(n + 2, v.body)
		elif isinstance(v, AST_RETURN):
			return ' ' * n + 'return' + '\n' + f(n + 2, v.val)
		else:
			return n * ' ' + str(v)
	for i in tree:
		s = f(0, i)
		print(s)

tree = parse(open('test_op.py').read())
show(tree)