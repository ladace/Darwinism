CFLAGS = $(INCLUDE_DIRS)
CXXFLAGS += -O2 -DLINUX -g -w -fmessage-length=0 $(INCLUDE_DIRS)
INCLUDE_DIRS = -Ilib -I../MyActionEditor -I../Behaviour -I../Vision -I../../../include -I../../../../Framework/include
LIBS += -lpthread -lncurses -lrt -ljpeg

OBJ = video.o robot.o jpeg_utils.o main.o

all: server

server: $(OBJ) lib actionEditor.o Vision.o darwin.a behav_lib
	$(CXX) -o server $(OBJ) ../Vision/Vision.o lib/httpd.a ../MyActionEditor/actionEditor.o ../Behaviour/lib.a ../../../lib/darwin.a $(LIBS)

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

Vision.o:
	$(MAKE) Vision.o -C ../Vision

clean:
	@rm *.o server

.PHONY: all clean run lib actionEditor.o vision.o

