import sys
import os
import thread
import threading
from subprocess import Popen
from multiprocessing import Process

if __name__ == '__main__':
    commands = []
    env = sys.argv[1]
    if (env == "local"):
        commands.append("./client localhost:12040 4096 test2.txt 41231233 9 < eightydays.txt");
        commands.append("./client localhost:12040 4096 test4.txt 41231233 9 < eightydays.txt");
        commands.append("./client localhost:12040 4096 test4.txt 41231233 9 < eightydays.txt");
    elif(env == "george" or env == "fred" or env == "percy" or env == "ginny"):
        commands.append("./client "+env+".socs.uoguelph.ca:12040  < kingsquest.txt");
        commands.append("./client "+env+".socs.uoguelph.ca:12040  < startrek.txt");
        commands.append("./client "+env+".socs.uoguelph.ca:12040  < eightydays.txt");
        
    processes = [Popen(cmd, shell=True) for cmd in commands]




     



