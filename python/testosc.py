# client example
import time
import liblo

target = liblo.Address("192.168.1.93",1234)
while(1):
    for p in xrange(0, 30):
        liblo.send(target, "/rgb/", p, 0, 0, 0)
        time.sleep(.005)
    for p in xrange(0, 30):
        c = 1;
        while(c < 255):
            c += 1 
            liblo.send(target, "/rgb/", p, c, c, c)
            time.sleep(.007)
            if(4 < p):
                liblo.send(target, "/rgb/", p - 4, 255 - c, 255 - c, 255 - c)
                time.sleep(.005)
