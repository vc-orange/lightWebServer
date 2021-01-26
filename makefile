TEST = ./test/lwsTest02.cpp

LIB_SRC = lwsChannel.o lwsEventLoop.o LwsPoller.o 
LIB_H = lwsChannel.h lwsEventLoop.h lwsLog.h
CXXFALGS = -O0 -g -Wall  -I -pthread
LDFLAGS = -lpthread

get: target

target: $(TEST) $(LIB_SRC)
	g++  $(CXXFALGS) -o TEST.o $(TEST) $(LIB_SRC) $(LDFLAGS)


lwsChannel.o:lwsChannel.cpp $(LIB_H)
	g++ -g -c lwsChannel.cpp -o lwsChannel.o

lwsEventLoop.o:lwsEventLoop.cpp $(LIB_H)
	g++ -g -c lwsEventLoop.cpp -o lwsEventLoop.o

LwsPoller.o :lwsPoller.cpp $(LIB_H)
	g++ -g -c LwsPoller.cpp -o LwsPoller.o 





clean:
	-rm  *.o



