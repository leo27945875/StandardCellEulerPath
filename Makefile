FILE_NUM   = $(i)
CPP_FILES  = ${wildcard *.cpp}
HDR_FILES  = ${wildcard include/*.hpp}
LAB        = Lab3
STUDENT_ID = 311581017

CXX_FLAGS  = -O3


main: ${CPP_FILES} ${HDR_FILES} euler_path.o structure.o utils.o
	g++ ${CXX_FLAGS} main.cpp euler_path.o structure.o utils.o -o ${LAB}

euler_path.o: euler_path.cpp ${HDR_FILES}
	g++ ${CXX_FLAGS} -c euler_path.cpp

structure.o: structure.cpp ${HDR_FILES}
	g++ ${CXX_FLAGS} -c structure.cpp

utils.o: utils.cpp include/utils.hpp
	g++ ${CXX_FLAGS} -c utils.cpp

clean:
	rm *.o ${LAB}

test:
	make
	./${LAB}