import spi
import time
import liblo

spi.openSPI(speed=1000000)


server = liblo.Server(1234)

def setrgb(path, args):
    p, r, g, b = args
    spi.transfer((p, r, g, b))
    print "setting %d to (%d,%d,%d)" % (p, r, g, b)

 
server.add_method("/rgb/", 'iiii', setrgb)

while True:
    server.recv(100)


