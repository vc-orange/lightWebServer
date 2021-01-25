objects = ./test/LELtest01.o lwsEventLoop.o 

buildv1 : $(objects)
	g++ -o TEST.o $(objects) -lpthread

./test/LELtest01.o : ./test/LELtest01.cpp lwsEventLoop.h
	g++ -g -c ./test/LELtest01.cpp -o ./test/LELtest01.o 

lwsEventLoop.o : lwsEventLoop.cpp lwsEventLoop.h
	g++ -g -c lwsEventLoop.cpp	 
lwslog.o : lwslog.h lwslog.cpp
	g++ -g -c lwslog.cpp

/test/threadPoolTest.o : ./test/threadPoolTest.cpp lwsThreadPool.h
	g++ -g -c ./test/threadPoolTest.cpp
lwsThreadPool.o :  lwsThreadPool.h lwsThreadPool.cpp
	g++ -g -c lwsThreadPool.cpp
clean:
	-rm TEST.o $(objects)