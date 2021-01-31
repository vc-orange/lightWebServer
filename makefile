TEST = ./test/lwsTest08.cpp
TEST2 = 
LIB_SRC = lwsChannel.o lwsEventLoop.o lwsPoller.o lwsTimer.o\
	lwsTimerStamp.o lwsTimerQueue.o lwsTimerId.o lwsEventLoopThread.o\
	lwsSocket.o lwsAcceptor.o lwsTcpConnection.o lwsTcpServer.o
LIB_H = lwsChannel.h lwsEventLoop.h lwsLog.h \
 lwsTimeStamp.h lwsTimerQueue.h lwsCallbacks.h lwsPoller.h \
 lwsTimer.h lwsTimerId.h lwsEventLoopThread.h\
 lwsSocket.h lwsAcceptor.h lwsTcpConnection.h lwsTcpServer.h
CXXFALGS = -O0 -g -Wall  -I -pthread
CLF = g++ -g -c #CXX LIB FALGS
LDFLAGS = -lpthread

get: target

target: $(TEST) $(LIB_SRC)
	g++  $(CXXFALGS) -o TEST.o $(TEST) $(LIB_SRC) $(LDFLAGS)

target2: $(TEST2) $(LIB_SRC)
	g++  $(CXXFALGS) -o TEST2.o $(TEST2) $(LIB_SRC) $(LDFLAGS)

lwsChannel.o:lwsChannel.cpp $(LIB_H)
	g++ -g -c lwsChannel.cpp -o lwsChannel.o

lwsEventLoop.o:lwsEventLoop.cpp $(LIB_H)
	g++ -g -c lwsEventLoop.cpp -o lwsEventLoop.o

lwsPoller.o :lwsPoller.cpp $(LIB_H)
	g++ -g -c LwsPoller.cpp -o LwsPoller.o 

lwsTimerStamp.o :lwsTimerStamp.cpp $(LIB_H)
	g++ -g -c lwsTimerStamp.cpp -o lwsTimerStamp.o

lwsTimerQueue.o :lwsTimerQueue.cpp $(LIB_H)
	$(CLF) lwsTimerQueue.cpp -o lwsTimerQueue.o

lwsTimerId.o :lwsTimerId.cpp $(LIB_H)
	$(CLF) lwsTimerId.cpp -o lwsTimerId.o

lwsTimer.o :lwsTimer.cpp $(LIB_H)
	$(CLF) lwsTimer.cpp -o lwsTimer.o

lwsEventLoopThread.o :lwsEventLoopThread.cpp $(LIB_H)
	$(CLF) lwsEventLoopThread.cpp -o lwsEventLoopThread.o

lwsSocket.o :lwsSocket.cpp $(LIB_H)
	$(CLF) lwsSocket.cpp -o lwsSocket.o

lwsAcceptor.o :lwsAcceptor.cpp $(LIB_H)
	$(CLF) lwsAcceptor.cpp -o lwsAcceptor.o

lwsTcpConnection.o :lwsTcpConnection.cpp $(LIB_H)
	$(CLF) lwsTcpConnection.cpp -o lwsTcpConnection.o

lwsTcpServer.o :lwsTcpServer.cpp $(LIB_H)
	$(CLF) lwsTcpServer.cpp -o lwsTcpServer.o

clean:
	-rm  *.o
	



