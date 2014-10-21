import Tkinter, tkFileDialog, tkFont

class App():
    
    def load(fname):
        return open(fname,'rb').read()

    def openfile(self):
        fp = tkFileDialog.askopenfile()
        self.textView.delete(0.0 , Tkinter.END)
        self.txt = fp.read()
        self.charslen = len(self.txt)
        self.textView.insert(0.0, self.txt)
        

    def scale_content(sc):
        val = int(sc)
        print(val)

    def filemenu(self):
        filebtn = Tkinter.Menu(self.menuframe)
        filebtn.add_command(label="open",command=self.openfile)
        return filebtn

    def __init__(self, title = 'Example'):
        self.charslen = 0.0
        root = Tkinter.Tk()
        root.minsize(600, 500)
        root.maxsize(600, 500)
        root.title(title)
        self.root = root
        # menu
        menuframe = Tkinter.Menu(root)
        self.menuframe = menuframe
        menuframe.add_cascade(label="file", menu = self.filemenu())
        root.config(menu = menuframe)

        # main
        mainframe = Tkinter.Frame(root)
        mainframe.pack(fill = Tkinter.BOTH, side = Tkinter.TOP)

        self.font = tkFont.Font(family = 'Consolas', size = 10)
        
        textView = Tkinter.Text(mainframe, wrap=Tkinter.WORD,
                                highlightthickness=0)
        textView.config( background ='black', foreground = 'green',
                         insertbackground='white', font = self.font)
        
        
        scale = Tkinter.Scrollbar(mainframe, orient = Tkinter.VERTICAL,
                                  command = textView.yview)
        scale.pack(side = Tkinter.RIGHT, fill = Tkinter.Y)
        # set textview scroll
        textView.config( yscrollcommand=scale.set)
        textView.pack(side = Tkinter.LEFT)
        self.textView = textView
        
    #  mainloop
    def run(self):
        self.root.mainloop()
    

app = App('HEX READER')
app.run()
