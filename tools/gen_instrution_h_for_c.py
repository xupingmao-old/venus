from instruction import *
import time, os


def check_file():
	global fp
	try:
		fp = open('instruction.h', 'r')
		tmp = open('instruction.h.bak', 'w')
		tmp.write(fp.read())
		tmp.close()
		fp.close()
	except:pass
	fp = open('instruction.h', 'w')

def close_file():
	fp.close()
def append(s = ""):
	fp.write(s+'\n')

def gen():
	check_file()
	g = globals()
	append('#ifndef INSTRUCTION_H_')
	append('#define INSTRUCTION_H_')
	append('/*')
	append('* @author xupingmao<578749341@qq.com>')
	append('* @since ' + time.asctime())
	append('*/')
	for i in g:
		if i.isupper():
			append('#define '+ i + ' = ' + str(g[i]))
	append()
	append('#endif')
	close_file()

gen()