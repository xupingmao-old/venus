def str_ljust(self, num):
    if len(self) >= num: return self
    rest = num - len(self)
    return self + rest * ' '

add_type_method('str', 'ljust', str_ljust)

def str_rjust(self, num):
    if len(self) >= num: return self
    rest = num - len(self)
    return rest * ' ' + self
add_type_method('str', 'rjust', str_rjust)

def _import(fname, tar = None):
    if fname in __modules__:
        pass
    else:
        from encode import *
        _code = b_compile(fname + '.py' )
        save(fname+".tmc", _code)
        load_module(fname, _code)
    g = __modules__[fname]
    if tar == '*':
        lg = get_last_frame_globals()
        for k in g:
            if k != '__name__':
                lg[k] = g[k]

__builtins__['_import'] = _import
def _execute_file(fname):
    from encode import *
    _code = b_compile(fname)
    load_module(fname, _code, '__main__')
    
def makesure( v , expect = None, cur = None):
    if not v and cur:
        _raise('error at ' + str( cur.pos ) + ' expect ' + expect + ' but see ' + cur.val)
__builtins__['makesure'] = makesure    
def _trace_execute_file(fname):
    from encode import *
    _code = b_compile(fname)
    from dis import *
    dis(_code)
    load_module(fname, _code, '__main__')

def _run_code(fname):
    txt = load(fname)
    _run(txt)

def _eval( fname ):
    from encode import *
    txt = load(fname)
    _code = b_compile(fname)
    _run(_code)