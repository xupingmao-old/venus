from tokenize import *

class ParserCtx:
	def __init__(self, r):
		r.append(Token(type='eof', val=None))
		self.r = r
		self.i = 0
		self.l = len(r)
		self.tree = []
	def next(self):
		if self.i < self.l:
			return self.r[self.i]
		return None
	def nextName(self):
		v = self.next()
		assert v.type == 'name'
		return v
	def nextSymbol(self):
		v = self.next()
		assert v.type == 'symbol'
		return v
	def add(self, v):
		self.tree.append(v)

def parse(v):
	r = tokenize(v)
	p = ParserCtx(r)
	do_prog(p)

def do_prog(p):
	r = p.nextSymbol()
	v = r.val
	while v != 'eof':
		if v == 'from':
			do_from(p)
		elif v == 'def':
			do_def(p)
def do_import(p):
	x = p.
def do_from(p):
	x = p.nextName()
	y = do_import(p)
	p.add(['from', x ])