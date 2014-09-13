from tokenize import *
from parse import *
r = tokenize(open('tokenize.py').read())

#for i in r:
#	print i.type, i.val

parse(open('test.py').read())