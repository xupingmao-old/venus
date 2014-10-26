
numbers = "1234567890"
alphas = "abcdefghijklmnopqrstuvwxyz"
symbols = "`!@#$%^&*(){}_+=-[]\\|:;'\"<>?,./ "
chars = numbers + alphas + alphas.upper() + symbols
class HexPaser:

    def __init__(self, fname, cols = 20 , gap = 5):
        if fname == None:
            return
        fp = open( fname, "rb")
        self.txt = fp.read()
        fp.close()
        self.fp = fp
        self.name = fname
        self.cols = cols
        self.gap = gap

    def getResult(self):
        des = ""
        count = 0
        temp = ""
        size = self.cols
        def hexchar(c):
            return hex(ord(c))[2:]
        line = ""
        for c in self.txt:
            if c in chars:
                temp+=c
            else:
                temp+='.'
            line += hexchar(c).center(3)
            count+=1
            if count % size == 0:
                des+= line + ' '* 5 + temp
                temp=""
                line = ""
                des+='\n'
        if line != "":
           des += line.ljust(size * 3) + ' '* self.gap + temp 
        return des

if __name__ == '__main__':
    import sys
    if len(sys.argv) == 2:
        fname = sys.argv[1]
    else:
        print("need filename")
        exit(0) 
    x = HexPaser(fname = fname)
    print(x.getResult())
