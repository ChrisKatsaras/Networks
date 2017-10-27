import sys
import os
if(sys.version_info[0] == 2):
    import thread
elif(sys.version_info[0] == 3):
    import _thread




from multiprocessing import Process

def func1(env):
    if (env == "local"):
        os.system("./client localhost:12040 4096 test1.txt 41231233 9 < alice.txt");
    elif(env == "george" or env == "fred" or env == "percy" or env == "ginny"):
        os.system("./client "+env+".socs.uoguelph.ca:12040  < alice.txt");

def func2(env):
    if (env == "local"):
        os.system("./client localhost:12040 4096 test2.txt 41231233 9 < kingsquest.txt");
    elif(env == "george" or env == "fred" or env == "percy" or env == "ginny"):
        os.system("./client "+env+".socs.uoguelph.ca:12040  < kingsquest.txt");
def func3(env):
    if (env == "local"):
        os.system("./client localhost:12040 4096 test3.txt 41231233 9 < startrek.txt");
    elif(env == "george" or env == "fred" or env == "percy" or env == "ginny"):
        os.system("./client "+env+".socs.uoguelph.ca:12040  < startrek.txt");
def func4(env):
    if (env == "local"):
        os.system("./client localhost:12040 4096 test4.txt 41231233 9 < eightydays.txt");
    elif(env == "george" or env == "fred" or env == "percy" or env == "ginny"):
        os.system("./client "+env+".socs.uoguelph.ca:12040  < eightydays.txt");
if __name__ == '__main__':
    p1 = Process(target=func1(sys.argv[1]))
    p1.start()
    p2 = Process(target=func2(sys.argv[1]))
    p2.start()
    p3 = Process(target=func3(sys.argv[1]))
    p3.start()
#    p4 = Process(target=func4(sys.argv[1]))
 #   p4.start()
    p1.join()
    p2.join()
    p3.join()
   # p4.join()

