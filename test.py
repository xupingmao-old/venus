from tokenize import *
from parse import *

x = 10
l = [1,2]
x = l[0]


x = 10

r = tokenize(open('tokenize.py').read())

#for i in r:
#	print i.type, i.val

parse(open('test.py').read())