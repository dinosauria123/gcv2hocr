# -*- coding: utf-8 -*-

import tkinter as tk
from tkinter import filedialog,messagebox
import os, signal
import subprocess
import glob
import threading


ctext = ""

class Application(tk.Frame):

    def __init__(self, master=None):
        super().__init__(master)
        self.master = master
        self.create_widgets()

    def create_widgets(self):
        global var
        var = tk.StringVar()
        var.set("Select image directory")
        self.lbl = tk.Label(root, textvariable=var)
        self.lbl.place(x=50, y=10)

        self.button = tk.Button(root)
        self.button["text"] = "Set IMG Dir"
        self.button["command"] = self.thread
        self.button.place(x=50, y=50)

        self.button2 = tk.Button(root)
        self.button2["text"] = "Config"
        self.button2["command"] = self.create_window
        self.button2.place(x=190, y=50)

        self.quit = tk.Button(root, text="Quit")
        self.quit["command"] = self.appQuit
        self.quit.place(x=300, y=50)


    def thread(self):
        global th
        th = threading.Thread(target=self.makepdf)
        th.start()

    def appQuit(self):
        os.kill(os.getpid(), signal.SIGKILL)

    def makepdf(self):
        path = os.path.dirname(__file__)+"/config.txt"
        try:
            with open(path) as f:
                APIKEY = f.read()
        except FileNotFoundError:
            messagebox.showerror("Error", 'No config file, Input your API key')
            self.create_window()
            exit(1)
        if APIKEY=="":
            messagebox.showerror("Error", 'No API key, Input your API key')

        dir = os.path.dirname(__file__)
        if glob.glob(dir+"/hocr-pdf.py") == []:
            messagebox.showerror("Error", 'No hocr-pdf.py. \nCheck hocr-pdf.py and this file \nare in the same directory.')
            exit(1)
        dname = filedialog.askdirectory(initialdir = dir)
        
        try:
            files = sorted(glob.glob(dname+"/*jpg"))
        except TypeError:                                  # cancel
            exit(1)

        gcvocr = os.path.dirname(__file__)+"/gcvocr.sh"

        for name in files:
            print("google OCR",os.path.basename(name))
            var.set("google OCR "+os.path.basename(name))
            command = ["sh", gcvocr, name, APIKEY]
            subprocess.call(command)
 
            print ("Convert ", os.path.basename(name) ,"to hocr")
            var.set("Convert " + os.path.basename(name) + " to hocr")
            hocr = name.replace("jpg", "hocr")
            json = name.replace("jpg", "jpg.json")
            command = ["gcv2hocr", json, hocr]
            subprocess.call(command)

        print("Generating out.pdf")
        var.set("Generating out.pdf")
        hocr_pdf = os.path.dirname(__file__)+"/hocr-pdf.py"
        out0 = dname+"/out0.pdf"

        command = ["python3", hocr_pdf, "--savefile", out0, dname]
        subprocess.call(command)

        print("Reducing pdf size")
        var.set ("Reducing pdf size")

        out = "-sOutputFile=" + dname + "/out.pdf"
        command = ["gs", "-sDEVICE=pdfwrite", "-dCompatibilityLevel=1.4", "-dPDFSETTINGS=/default", "-dNOPAUSE", "-dQUIET", "-dBATCH", "-dAutoRotatePages=/None", out, out0]
        subprocess.call(command)
        print("Done!")
        var.set("Done!")

    def create_window(self):

        global t, text
        t = tk.Toplevel(self)
        t.geometry("350x100")
        t.wm_title("API KEY Config")
        l = tk.Label(t, text="Set Google API key")
        l.place(x=100, y=5)
        text = tk.Entry(t, width=20)
        text.place(x=90, y=30)
        button3 = tk.Button(t)
        button3.place(x=150, y=60)
        button3["text"] = "Save"
        button3["command"] = self.saveconfig

    def saveconfig(self):
        ctext = text.get()
        if ctext == "":
            messagebox.showerror("Error", "No API KEY") 
            t.destroy()
            exit(1)
        path = os.path.dirname(__file__)+"/config.txt"
        with open(path, mode='w') as f:
            f.write(ctext)
        t.destroy()

root = tk.Tk()
root.geometry("400x100")
root.title("Make pdf")
app = Application(master=root)
app.mainloop()
