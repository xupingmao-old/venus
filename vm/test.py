if str(1.0) != '1':
    import time
    clock = time.time
for i in range(1, 10):
	print("now is "+str(i))
	i+=1

def add(a,b):
    return a + b

print( add( 10, 20 ))

class Test:
    def __init__(self, name, age):
        self.name = name
        self.age = age
        self.tell(self)
    def grow(self):
        self.age += 1
    
    def tell(self):
        print('name is '+ self.name, 'age is ' + str(self.age))

x = Test('test',20)
x.grow()
x.tell()
print("'-'.join(['1','2','3'])", '-'.join(['1','2','3']))
print("'-'.join('hello,world,yeah').split(',')", '-'.join( 'hello,world,yeah'.split(',') ))

print("test list ")
print([3,4,5])
print(mtime('object.h'))

omap = {"test": "4","age":"3434"}
print(omap)
print("hello,world")
print(ARGV)
print(__name__)
