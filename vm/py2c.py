from encode import *

def py2bin(srclist, des):
    if istype(srclist, 'string'):
        srclist = [srclist]
    txt = ""
    for src in srclist:
        code = b_compile(src)
        txt += "unsigned char " + src.replace(".py", "_pyc")+ "[] = {"
        codes = []
        for c in code:
            codes.append( str(ord(c)))
        txt += ','.join(codes) + "};\n"
    save(des, txt)

def b_import(file):
    if file in MODULES:
        return
    code = b_compile(file)
    save( file.replace('.py', '.pyc'), code)

if __name__ == "__main__":
    py2bin(["_boot.py", "tokenize.py", "parse.py", "instruction.py", "encode.py"], "compile.c")

