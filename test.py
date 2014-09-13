from tokenize import *

r = tokenize(open('tokenize.py').read())

for i in r:
	print i.type, i.val