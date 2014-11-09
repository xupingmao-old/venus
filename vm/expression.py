#
# 
#
#
#
#
#
#
class AstNode:
    def __init__(self, type=None):
        self.type = type
def factor_next_if(p):
    if p.token.type == 'if':
        p.next()
        node = AstNode()
        node.type = 'choose'
        expr(p)
        node.cond = p.pop()
        node.left = p.pop()
        p.expect('else')
        expr(p)
        node.right = p.pop()
        p.add(node)
factor_op_list = ['number', 'string', 'name', 'None']
def factor(p):
    t = p.token.type
    token = p.token
    if t in factor_op_list:
        p.next()
        p.add(token)
        factor_next_if(p)
    elif t == '[':
        p.next()
        node = AstNode()
        node.type = 'list'
        if p.token.type == ']':
            p.next()
            node.val = None
        else:
            expr(p)
            p.expect(']')
            node.val = p.pop()
        p.add( node )
    elif t == '(':
        p.next()
        expr(p)
        p.expect(')')
    elif t == '{':
        p.next()
        node = AstNode('dict')
        if p.token.type == '}':
            p.next()
            node.items = None
            p.add(node)
        else:
            items = []
            while p.token.type != '}':
                or_exp(p)
                p.expect(':')
                or_exp(p)
                v = p.pop()
                k = p.pop()
                items.append([k,v])
                if p.token.type == '}':
                    break
                p.expect(',')
            p.expect('}')
            node.items = items
            p.add(node)
            
assign_op_list = ['=', '+=', '-=', '*=', '/=', '%=']
def assign_exp(p):
    fnc = comma_exp
    fnc(p)
    if p.token.type in assign_op_list:
        t = p.token.type
        p.next()
        fnc(p)
        p.addOp(t)


def comma_exp(p):
    fnc = or_exp
    fnc(p)
    while p.token.type == ',':
        t = p.token.type
        p.next()
        fnc(p)
        p.addOp(t)
        
def or_exp(p):
    fnc = and_exp
    fnc(p)
    while p.token.type == 'or':
        p.next()
        fnc(p)
        p.addOp('or')

def and_exp(p):
    fnc = not_exp
    fnc(p)
    while p.token.type == 'and':
        p.next()
        fnc(p)
        p.addOp('and')

def not_exp(p):
    fnc = cmp_exp
    if p.token.type == 'not':
        p.next()
        not_exp(p)
        node = AstNode('not')
        node.val = p.pop()
        p.add(node)
    else:
        fnc(p)
def pre_exp(p):
    pass

cmp_op_list = ['>', '<', '==', '!=', '>=', '<=', 'in', 'notin']
def cmp_exp(p):
    fnc = item
    fnc(p)
    while p.token.type in cmp_op_list:
        t = p.token.type
        p.next()
        fnc(p)
        p.addOp(t)

item_op_list = ['+', '-']
def item(p):
    fnc = item2
    fnc(p)
    while p.token.type in item_op_list:
        t = p.token.type
        p.next()
        fnc(p)
        p.addOp(t)

item2_op_list = ['*','/', '%']
def item2(p):
    fnc = call_or_get_exp
    fnc(p)
    while p.token.type in item2_op_list:
        t = p.token.type
        p.next()
        fnc(p)
        p.addOp(t)
        

first_op_list = ['.','(','[']
def call_or_get_exp(p):
    if p.token.type == '-':
        p.next()
        call_or_get_exp(p)
        node = AstNode('neg')
        node.val = p.pop()
        p.add(node)
    else:
        factor(p)
        while p.token.type in first_op_list:
            t = p.token.type
            p.next()
            if t == '[':
                expr(p)
                p.expect(']')
                p.addOp('get')
            elif t == '(':
                node = AstNode('$')
                node.name = p.pop()
                if p.token.type == ')':
                    p.next()
                    node.args = None
                    p.add( node )
                else:
                    comma_exp(p)
                    p.expect(')')
                    node.args = p.pop()
                    p.add( node )
            # handler dot (.)
            else:
                factor(p)
                b = p.pop()
                b.type = 'string'
                a = p.pop()
                node = AstNode('get')
                node.a = a
                node.b = b
                p.add(node)
expr = assign_exp
