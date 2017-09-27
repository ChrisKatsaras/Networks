import os
import thread

commands = ['./client 127.0.0.1:2300 &', './client 127.0.0.1:2300 &', './client 127.0.0.1:2300 &', './client 127.0.0.1:2300 &', './client 127.0.0.1:2300 &']
for com in commands:
    os.system(com) # now commands will run in background