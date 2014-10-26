from tokenize import *
from parse import *
from boot import *

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

ops_list = ['from', '+', '-', '*', '/', '%', ',' ,'=', 
		'+=', '-=', '/=', '*=', 'get',
		"==", "!=", ">", "<", ">=", "<=", "and",
		 "or", "for","while", "in", "notin"]

def show(tree):
	if not istype(tree, 'list'):
		return
	#print(self.tree)
	def f(n, v, pre = ""):
		rs = ''
		if v == None:
			rs = 'None'
		elif isinstance(v, list):
			s = ''
			for i in v:
				s += '\n' + f(n+2, i)
			rs = s
		elif isinstance(v, Token):
			if v.type == 'string':
				rs = "'" + sp_str(v.val) + "'"
			elif v.type == 'number':
				rs = sp_str(v.val)
			elif v.type == 'name':
				rs = sp_str(v.val)
			elif v.type in ['neg', 'pos', 'not', 'list']:
				rs = v.type + '\n' + f(n+2, v.val)
		else:
			if v.type in ['neg', 'pos', 'not', 'list']:
				rs = v.type + '\n' + f(n+2, v.val)
			elif v.type in ops_list:
				rs = v.type + '\n' + f(n+2, v.a) + '\n' + f(n+2, v.b)
			elif v.type == '$':
				rs = 'invoke\n' + f(n+2, v.name) + '\n' + f(n+2, v.args)
			elif v.type in ['if', 'choose']:
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
			elif v.type in ('return', 'global', 'raise'):
				rs = v.type + '\n' + f(n+2, v.val)
			elif v.type in ("break", "continue", "pass"):
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
	for i in tree:
		s = f(0, i)
		print(s)
x = 0
x = 12 if x > 10 else 3
# tree = parse(open('parse.py').read())

def main():
	import sys
	argv = sys.argv
	if len(argv) > 1:
		f = argv[1]
	else:
		f = 'parse.py'
	tree = parse( load(f) )
	show(tree)

main()
# import json

# txt = json.dumps(tree)
# print(txt)
