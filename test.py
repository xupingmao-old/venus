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
	def f(n, v, pre = ""):
		rs = ''
		if isinstance(v, list):
			s = ''
			for i in v:
				s += '\n' + f(n+2, i)
			rs = s
		elif isinstance(v, Token):
			if v.type == 'string':
				rs = "'" + v.val + "'"
			else:rs = str(v.val)
		elif isinstance(v, AstNode):
			if v.type in ['neg', 'pos', 'not', 'list']:
				rs = v.type + '\n' + f(n+2, v.val)
			elif v.type in ['from', '+', '-', '*', '/', '%', ',' ,'=', '+=', '-=', '/=', '*=', 'get', 'attr', 
					"==", "!=", ">", "<", ">=", "<=", "and", "or"]:
				rs = v.type + '\n' + f(n+2, v.a) + '\n' + f(n+2, v.b)
			elif v.type == '$':
				rs = 'invoke\n' + f(n+2, v.name) + '\n' + f(n+2, v.args)
			elif v.type == 'if':
				rs = 'if\n' + f(n+2, v.cond, 'cond => ') + \
					'\n' + f(n+2, v.body, 'body => ') + '\n' + f(n+2, v._else, 'else => ')
			elif v.type == 'def':
				rs = 'def ' + f(0 , v.name , 'name => ') + \
					'\n' + f(n+2, v.args, 'args => ') + '\n' + f(n+2, v.body, "body => ")
			elif v.type in ['varg', 'arg']:
				rs = v.type + f(1 , v.name, 'name => ') + f(1,  v.val, 'dafault => ')
			else:
				rs = v.type
		else:
			rs = str(v)
		return ' ' * n + pre + rs
	for i in tree:
		s = f(0, i)
		print(s)

tree = parse(open('test_op.py').read())
show(tree)
