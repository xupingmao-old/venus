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
        # print("module "+fname+" already loaded");
        pass
    else:
        # print("load module "+fname)
        from encode import *
        txt = load(fname)
        # print("src:\n"+txt)
        _code = b_compile(fname)
        if fname != ARGV[0]:
            save(fname+".tmc", _code)
        load_module(fname, _code)
    g = __modules__[fname]
    if tar == '*':
        lg = get_last_frame_globals()
        for k in g:
            lg[k] = g[k]

__builtins__['_import'] = _import
def _execute_file(fname):
    from encode import *
    _code = b_compile(fname)
    load_module('__main__', _code)


def _run_code(fname):
    txt = load(fname)
    _run(txt)

def _eval( fname ):
    from encode import *
    txt = load(fname)
    _code = b_compile(fname)
    _run(_code)