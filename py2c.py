from encode import *

def py2bin(srclist, des):
    if istype(srclist, 'string'):
        srclist = [srclist]
    txt = ""
    for src in srclist:
        tempfile = src+".bin"
        save_bin(src, tempfile)
        v = load(tempfile)
        txt += "unsigned char " + src.replace(".py", "_pyc")+ "[] = {"
        codes = []
        for c in v:
            codes.append( str(ord(c)))
        txt += ','.join(codes) + "};\n"
        rm(tempfile)
    save(des, txt)

def _import(file):
    if file in MODULES:
        return
    save_bin(file, file+".bin")

if __name__ == "__main__":
    py2bin(["tokenize.py", "parse.py", "instruction.py", "encode.py"], "compile.c")

