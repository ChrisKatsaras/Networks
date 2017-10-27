
import os
import thread
import sys

from multiprocessing import Process

def func1(env):
    os.system("./client percy.socs.uoguelph.ca:12040 < eightydays.txt >> output.csv");
if __name__ == '__main__':
    
    for i in range(0, 20):
        exec('p' + str(i) + ' = Process(target=func1(sys.argv[1]))')
        exec('p' + str(i) + '.start()')
        exec('p' + str(i) + '.join()')
       
