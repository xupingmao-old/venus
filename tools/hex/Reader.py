import Tkinter, tkFileDialog, tkFont
from HexPaser import HexPaser
import os
class App():
    
    def load(fname):
        return open(fname,'rb').read()

    # update filename and filetext
    def update(self):
        self.textView.delete(0.0, Tkinter.END)
        self.textView.insert(0.0, self.txt)
        title = self.title + ' -- ' + self.name
        self.root.title(title)
        # bottom text
        btext = self.name[:50]
        btext = btext.ljust(50)
        btext += '    ' + str(os.path.getsize(self.name))+' byte'
        self.filelabel.config( text = btext)
    def openfile(self):
        # this open mode is proberly r, not rb
        fp = tkFileDialog.askopenfile()
        # do not select any file
        if fp == None:
            return
        self.name = fp.name
        x = HexPaser(fp.name, self.cols, self.gap)
        self.txt = x.getResult()
        fp.close()
        self.update()
    def openastext(self):
        fp = tkFileDialog.askopenfile()
        if fp == None:
            return
        self.txt = fp.read()
        self.name = fp.name
        fp.close()
        self.update()

    def scale_content(sc):
        val = int(sc)
        print(val)

    def filemenu(self):
        filebtn = Tkinter.Menu(self.menuframe)
        filebtn.add_command(label="open as hex",command=self.openfile)
        filebtn.add_command(label="open as text", command=self.openastext)
        filebtn.add_command(label="exit",command=self.exit) 
        return filebtn
    def exit(self):
        self.root.quit()
        exit(0)
    def exitmenu(self):
        btn = Tkinter.Menu(self.menuframe)
        btn.add_command( label = "exit", command = self.exit )
        return btn

    def get_cols_text(self):
        hex_cols = ''
        txt_cols = ''
        for i in range(1, self.cols+1):
            hex_cols+=str(i).ljust(3)
        return hex_cols

    def __init__(self, title = 'Example', cols = 20, gap = 5, convertTab = True):
        self.title = title
        self.convertTab = convertTab
        self.charslen = 0.0
        self.cols = cols
        self.gap = gap

        root = Tkinter.Tk()
        width = 900
        root.minsize(width, 500)
        root.maxsize(width, 500)
        root.title(title)
        self.root = root
        # menu
        menuframe = Tkinter.Menu(root)
        self.menuframe = menuframe
        menuframe.add_cascade(label="file", menu = self.filemenu())
        root.config(menu = menuframe)

        # colums number
##        colsFrame = Tkinter.Frame(root)
##        self.colsFrame = colsFrame
##        colsFrame.pack( fill = Tkinter.X, side = Tkinter.TOP)
##        colsLabel = Tkinter.Label(colsFrame)
##        colsLabel.config(text = self.get_cols_text())
##        colsLabel.pack(fill = Tkinter.X, side = Tkinter.LEFT)

        # main
        mainframe = Tkinter.Frame(root)
        mainframe.pack(fill = Tkinter.X, side = Tkinter.TOP)

        self.font = tkFont.Font(family = 'Consolas', size = 10)

        # main text view
        textView = Tkinter.Text(mainframe, wrap=Tkinter.WORD,
                                highlightthickness=0, width=width)
        textView.config( background ='black', foreground = 'green',
                         insertbackground='white', font = self.font)
        
        scale = Tkinter.Scrollbar(mainframe, orient = Tkinter.VERTICAL,
                                  command = textView.yview)
        scale.pack(side = Tkinter.RIGHT, fill = Tkinter.Y)
        # set textview scroll
        textView.config( yscrollcommand=scale.set)
        textView.pack(side = Tkinter.LEFT)
        self.textView = textView

        # bottom information
        self.bottom = Tkinter.Frame(root)
        self.bottom.pack(fill = Tkinter.X, side = Tkinter.BOTTOM)
        filelabel = Tkinter.Label(self.bottom, text = 'file')
        filelabel.pack(side = Tkinter.LEFT)
        self.filelabel = filelabel
        
    #  mainloop
    def run(self):
        self.root.mainloop()
    

app = App('HEX READER')
app.run()
