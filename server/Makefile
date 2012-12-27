CFLAGS = $(INCLUDE_DIRS)
CXXFLAGS += -O2 -DLINUX -g -w -fmessage-length=0 $(INCLUDE_DIRS)
INCLUDE_DIRS = -Ilib -I../MyActionEditor -I../Behaviour -I../../../include -I../../../../Framework/include
LIBS += -lpthread -lncurses -lrt

all: server

server: main.o robot.o video.o lib actionEditor.o darwin.a behav_lib
	$(CXX) -o server main.o robot.o lib/httpd.a ../MyActionEditor/actionEditor.o ../Behaviour/lib.a ../../../lib/darwin.a $(LIBS)

run: server
	./server

lib:
	@cd lib && $(MAKE)

behav_lib:
	@cd ../Behaviour && $(MAKE) lib.a

darwin.a:
	$(MAKE) -C ../../../build

actionEditor.o:
	$(MAKE) actionEditor.o -C ../MyActionEditor

clean:
	@rm *.o server

.PHONY: all clean run lib

