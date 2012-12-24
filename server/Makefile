CXXFLAGS +=	-O2 -DLINUX -g -Wall -fmessage-length=0 $(INCLUDE_DIRS)
INCLUDE_DIRS = -Ilib -I../MyActionEditor -I../Behaviour
LIBS += -lpthread -lncurses -lrt

all: server

server: main.o lib actionEditor.o darwin.a
	$(CXX) -o server main.o lib/httpd.a ../MyActionEditor/actionEditor.o ../../../lib/darwin.a $(LIBS)

run: server
	./server

lib:
	@cd lib && $(MAKE)

darwin.a:
	$(MAKE) -C ../../../build

actionEditor.o:
	$(MAKE) actionEditor.o -C ../MyActionEditor

clean:
	@rm *.o server

.PHONY: all clean run

