from tokenize import *
from boot import *


r = tokenize(load(argv[1]))

for i in r:
    print(i)

# if x == 0:
#     print(x )
# elif x == 1:
#     print(1)
# elif x == 2:
#     print(2)
# else:
#     print('hello')

# def complex(a, b, c):
#     a = b * c 
#     c = a + b
#     return a + c

# def test():
# 	return 'hello,world'

# def test(a, b = 2, c = 4):
# 	pass

# x = test(1,3,4)
# def test3(a , b = 34):
# 	pass

# def test(*v):
# 	return v[0] + v[1]

# x = a + b

# def add(a, b):
# 	return a + b

# x = not not 1

# y = --- x + +++ 34

# def do_stm(p):
#     skip_nl(p)
#     t = p.token.type
#     if t == 'from':
#         do_from(p)
#     elif t == 'import':
#         do_import(p)
#     elif t == "assert":
#         do_assert(p)
#     elif t == 'def':
#         do_def(p)
#     elif t == 'class':
#         do_class(p)
#     elif t in ('for', 'while'):
#         do_for_while(p, t)
#     # elif t in ('return', "raise"):
#         do_stm1(p, t)
#         stm_next_if(p)
#     # elif t in ("break", "continue", "pass"):
#         p.next()
#         node = AstNode()
#         node.type = "pass"
#         p.add( node )
#     elif t == 'if':
#         do_if(p)
#     elif t == 'name':
#         expr(p)
#         stm_next_if(p)
#     else:
#         raise Exception('unknown expression, type = ' + t + ', pos = ' + str(p.token.pos))
#     skip_nl(p)
