class MyExpr:
    def __init__(self, fnc, range):
        self.fnc = fnc
        self.range = range

    def run(self, p):
        range = self.range
        self.fnc(p)
        while p.token.type in range:
            t = p.token.type
            # print(p.token.pos)
            p.next()
            self.fnc(p)
            p.addOp(t)

class Temp:
    def __init__(self,fnc, range):
        self.fnc = fnc
        self.range = range
    def run(self, p):
        if p.token.type == '-':
            p.next()
            self.run(p)
            node = AstNode("neg")
            node.val = p.pop()
            p.add(node)
            return
        self.fnc(p)
        while p.token.type in self.range:
            t = p.token.type
            p.next()
            if t == '[':
                expr(p)
                p.expect(']')
                p.addOp('get')
            elif t == '(':
                node = AstNode()
                node.type = '$'
                node.name = p.pop()
                if p.token.type == ')':
                    p.next()
                    node.args = None
                    p.add( node )
                else:
                    expr(p)
                    p.expect(')')
                    node.args = p.pop()
                    p.add( node )
            # handler dot (.)
            else:
                self.fnc(p)
                b = p.pop()
                b.type = 'string'
                a = p.pop()
                node = AstNode('get')
                node.a = a
                node.b = b
                p.add(node)
class PreExpr:
    def __init__(self, fnc, range):
        self.fnc = fnc
        self.range = range
    def run(self, p):
        if p.token.type in self.range:
            # print(p.token.pos)
            p.next()
            self.run(p)
            node = AstNode("not")
            node.val = p.pop()
            p.add(node)
        else:
            self.fnc(p)
dot_expr = Temp(factor, ['.', '[', '('])
item2 = MyExpr(dot_expr.run, ['*', '/', '%'])
item = MyExpr(item2.run, ['+', '-'])
in_expr = MyExpr(item.run, ['in', 'notin'] )
compare = MyExpr(in_expr.run,  ['>', '<', '>=', '<=', '==', '!=', 'is', 'isnot'])
pre_expr = PreExpr(compare.run, ['not']);
and_expr = MyExpr(pre_expr.run, ['and'])
or_expr = MyExpr(and_expr.run, ['or'])
comma = MyExpr(or_expr.run, [','])
assign = MyExpr(comma.run, ['=', '+=', '-=', '*=', '/=', '%='])

# dot_expr = _expr4(factor)
# item2 = _expr2(dot_expr, ['*', '/', '%'])
# item = _expr2(item2, ['+', '-'])
# in_expr = _expr2(item, ['in', 'notin'])
# compare = _expr2(in_expr, ['>', '<', '>=', '<=', '==', '!=', 'is', 'isnot'])
# and_expr = _expr2(compare, ['and'])
# or_expr = _expr2(and_expr, ['or'])
# comma = _expr2(or_expr, [','])
# assign = _expr2(comma, ['=', '+=', '-=', '*=', '/='])