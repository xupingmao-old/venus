
from configparser import *

class Conf:
    def __init__(self, file):
        conf = ConfigParser()
        conf.read( file )

    def setBase(self, base):
        self.base = base


conf = Conf("files.ini")