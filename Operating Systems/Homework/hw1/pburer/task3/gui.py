from tkinter import *
from subprocess import check_output
import sys

test = check_output('./manager').split(b'\n')

for i in range(len(test)):
    test[i] = test[i].decode(encoding='UTF-8', errors='ignore')
    if len(test[i]) > len("Executable path:"):
        if test[i][:len("Executable path:")] == "Executable path:":
            test[i] = test[i][:len(test[i])]

processes = []
for i in range(len(test)-1):
    if test[i] == "":
        new = []
        new.append(test[i-5].strip("pid = "))
        new.append(test[i-4].strip("Name:\t"))
        new.append(test[i-3].strip("voluntary_ctxt_switches:\t"))
        new.append(test[i-2].strip("nonvoluntary_ctxt_switches:\t"))
        new.append(test[i-1].strip("Executable path: "))
        processes.append(new)

root = Tk()

root.title("User Program Manager")

root.geometry('900x900')

lb = Label(root, text = "Welcome to the User Program Manager", width = 100, anchor = "w")
lb.grid()

all = Button(root, text = "Show all processes in order", fg = "red", width = 100, anchor = "w")
all.grid(column=0, row = 1)

EXITER = Button(root, text = "Exit the program", fg = "red", command = sys.exit, width = 100, anchor = "w")
EXITER.grid(column = 0, row = 9)

global k
k = 0
def show(b):
    global k
    if b:
        k += 1
    else:
        k -= 1
    process = processes[k]
    pid = Label(root, text = "pid = " + process[0], width = 100, anchor = "w", fg = 'green')
    pid.grid(column = 0, row = 3)

    name = Label(root, text = "Name = " + process[1], width = 100, anchor = "w", fg = 'purple')
    name.grid(column = 0, row = 2)

    v_ctxt = Label(root, text = "Vol. Context Switches = " + process[2], width = 100, anchor = "w", fg = 'red')
    v_ctxt.grid(column = 0, row = 4)

    nv_ctxt = Label(root, text = "Non-vol. Context Switches: " + process[3], width = 100, anchor = "w", fg = 'red')
    nv_ctxt.grid(column = 0, row = 5)

    exe_path = Label(root, text = "Executable path: " + process[4], width = 100, anchor = "w", fg = 'blue')
    exe_path.grid(column = 0, row = 6)
def alls():
    lb.configure(text = "Showing all processes", anchor = "w", width = 100)
    all.destroy()
    global k
    process = processes[k]
    pid = Label(root, text = "pid = " + process[0], width = 100, anchor = "w", fg = 'green')
    pid.grid(column = 0, row = 3)

    name = Label(root, text = "Name = " + process[1], width = 100, anchor = "w", fg = 'purple')
    name.grid(column = 0, row = 2)

    v_ctxt = Label(root, text = "Vol. Context Switches = " + process[2], width = 100, anchor = "w", fg = 'red')
    v_ctxt.grid(column = 0, row = 4)

    nv_ctxt = Label(root, text = "Non-vol. Context Switches: " + process[3], width = 100, anchor = "w", fg = 'red')
    nv_ctxt.grid(column = 0, row = 5)

    exe_path = Label(root, text = "Executable path: " + process[4], width = 100, anchor = "w", fg = 'blue')
    exe_path.grid(column = 0, row = 6)
    next = Button(root, text = "Next", fg = 'blue', command = lambda: show(True), width = 100)
    prev = Button(root, text = "Prev", fg = 'blue', command = lambda: show(False), width = 100)
    next.grid(column = 0, row = 8)
    prev.grid(column = 0, row = 7)
all["command"] = alls
root.mainloop()