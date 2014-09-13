from tokenize import *

class ParserCtx:
	def __init__(self, r):
		r.append(Token(type='eof', val=None))
		self.r = r
		self.i = 0
		self.l = len(r)
		self.tree = []
	def match(self, v):
		x = self.next()
		assert x.val == v
	def next(self):
		if self.i < self.l:
			self.i+=1
			return self.r[self.i-1]
		return None
	def nextis(self, v):
		if self.i < self.l:
			return self.r[self.i].val == v
		return False
	def back(self):
		if self.i != 0:
			self.i -= 1
	def nextName(self):
		v = self.next()
		assert v.type == 'name'
		return v
	def nextSymbol(self):
		v = self.next()
		assert v.type == 'symbol'
		return v
	def add(self, t, v = None, items=None):
		self.tree.append([t, v , items])
	def show(self):
		for i in self.tree:
			print i

def parse(v):
	r = tokenize(v)
	p = ParserCtx(r)
	do_prog(p)

def do_import(p):
	p.match('import')
	x = p.next()
	if x.type == 'symbol' and x.val == '*':
		return '*'
	elif x.type == 'name':
		items=[x]
		while p.nextis(','):
			p.next()
			items.append(p.nextName())
		return items
	raise 'error'
def do_from(p):
	x = p.nextName()
	y = do_import(p)
	p.add('from', x, y)

maps = {
	"from" : do_from,
	"import" : do_import
}

def do_prog(p):
	r = p.nextSymbol()
	while r.type != 'eof':
		if r.type == 'symbol':
			maps[r.val](p)
		elif r.type == 'name':
			print 'unknown'
		r = p.next()
	p.show()