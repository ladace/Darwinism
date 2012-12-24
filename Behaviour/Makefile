CXX = g++

TARGET = test

INCLUDE_DIRS = -I../../../include -I../../../../Framework/include
CXXFLAGS +=	-O2 -DLINUX -g -Wall -fmessage-length=0 $(INCLUDE_DIRS)
LIBS += -lpthread -lncurses -lrt

OBJS =	WalkerManager.o	\
		Behaviour.o	\
		HeadManager.o


all: darwin.a $(TARGET) lib.a

darwin.a:
	make -C ../../../build

lib.a:
	ar cr lib.a $(OBJS)

$(TARGET): $(OBJS) ../../../lib/darwin.a lib.a
	$(CXX) -o $(TARGET) main.o lib.a ../../../lib/darwin.a $(LIBS)
	
clean:
	rm -f $(OBJS) $(TARGET) lib.a





