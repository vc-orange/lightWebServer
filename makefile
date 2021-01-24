build : ./test/threadPoolTest.o lwsThreadPool.o 
	g++ -o TEST.o ./test/threadPoolTest.o lwsThreadPool.o -lpthread
/test/threadPoolTest.o : ./test/threadPoolTest.cpp lwsThreadPool.h
	g++ -g -c ./test/threadPoolTest.cpp
lwsThreadPool.o :  lwsThreadPool.h lwsThreadPool.cpp
	g++ -g -c lwsThreadPool.cpp
clean:
	rm TEST.o ./test/threadPoolTest.o lwsThreadPool.o