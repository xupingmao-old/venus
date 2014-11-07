if str(1.0) != '1':
    import time
    clock = time.time
#for i in range(1, 10):
	#print("now is "+str(i))
	#i+=1

if not True:
    print('www')
    
def add(a,b):
    return a + b

#print( add( 10, 20 ))

class Test:
    def __init__(self, name, age):
        self.name = name
        self.age = age
        self.tell()
    def grow(self):
        self.age += 1
    
    def tell(self):
        print('name is '+ self.name, 'age is ' + str(self.age))
i = 0
j = 0
t1 = clock()
while i < 1000000:
    j = i * 34
    i += 1
t2 = clock()
print("global scope: used time = " + str(t2-t1))
def test():
    i = 0;j = 0
    t1 = clock()
    while i < 1000000:
        j = i * 34
        i +=1
    t2 = clock()
    print("local scope: used time = " + str(t2-t1))
test()
# x = input("hold>>>")
# print("__objectsize__=",__objectsize__)
# print(est)
x = Test('test',20)
x.grow()
x.tell()
print("'-'.join(['1','2','3'])", '-'.join(['1','2','3']))
print("'-'.join('hello,world,yeah').split(',')", '-'.join( 'hello,world,yeah'.split(',') ))
print(x)

print("test list ")
print([3,4,5])
# print(mtime('object.h'))

omap = {"test": "4","age":"3434"}
print(omap)
print("hello,world")
#print(ARGV)
print(__name__)

def fib(n):
    if n == 0 or n == 1: return 1
    return fib(n-1) + fib(n-2)
t1 = clock()
#fib(30)
t2 = clock()
print("cursive call test : " , t2-t1)

