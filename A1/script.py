import os
import thread


#os.system("./client 127.0.0.1:2300");
#os.system("./client 127.0.0.1:2300");
#os.system("./client 127.0.0.1:2300");
#os.system("./client 127.0.0.1:2300");
#os.system("./client 127.0.0.1:2300");
#os.system("./client 127.0.0.1:2300");
#os.system("./client 127.0.0.1:2300");
#os.system("./client 127.0.0.1:2300");

from multiprocessing import Process

def func1():
  os.system("./client 127.0.0.1:2300 < test.txt");

def func2():
  os.system("./client 127.0.0.1:2300 < test.txt");

def func3():
  os.system("./client 127.0.0.1:2300 < test.txt");

def func4():
  os.system("./client 127.0.0.1:2300 < test.txt");    

if __name__ == '__main__':
  p1 = Process(target=func1)
  p1.start()
  p2 = Process(target=func2)
  p2.start()
  p3 = Process(target=func3)
  p3.start()
  p4 = Process(target=func4)
  p4.start()
  p1.join()
  p2.join()
  p3.join()
  p4.join()

