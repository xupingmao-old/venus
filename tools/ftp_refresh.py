from ftplib import FTP
from configparser import *
import time

def load_conf():
    global ip,port,user,password
    conf = ConfigParser()
    conf.read('ftp.ini')
    sec = 'ftp'
    ip=conf.get(sec,'ip')
    #port=int(conf.get(sec,'port'))
    user=conf.get(sec,'user')
    password=conf.get(sec,'password')
    print('==============config==============')
    print('%-10s:%-10s' % ('ip', ip))
    #print('%-10s:%-10d' % ('port', port))
    print('%-10s:%-10s' % ('user', user))
    print('%-10s:%-10s' % ('password', password))

def main():
    load_conf()
    f = FTP()
    f.connect(ip, 21)
    f.login(user, password)
    def callback(line):
        print(line[line.index(' '):])
    while 1:
        print(time.asctime())

        f.dir('.', callback)
        print()
        # seconds
        time.sleep(60 * 5)

main()
