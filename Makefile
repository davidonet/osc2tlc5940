#CXX =/opt/cross/x-tools/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-g++
CXX=g++
CXXFLAGS =-O2 -g -Wall -fmessage-length=0 -Ibcm2835/src

OBJS =	osc2tlc5940.o

LIBS = bcm2835/src/bcm2835.c -fpermissive

TARGET = osc2tlc5940

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
