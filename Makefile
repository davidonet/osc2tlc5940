CXX =/opt/cross/x-tools/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-g++
#CXX=g++
CXXFLAGS =-O2 -g -Wall -fmessage-length=0 -Ibcm2835/src -Iliblo

OBJS =	osc2tlc5940.o

LIBS = bcm2835/src/bcm2835.c -fpermissive -pthread liblo/src/.libs/liblo.a

TARGET = osc2tlc5940

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)
	scp osc2tlc5940 pi@192.168.1.16:/home/pi/osc2tlc5940

clean:
	rm -f $(OBJS) $(TARGET)
