## file Makefile
CXXOBJECTS= %.o
CXX= g++ 
CXXSOURCES= main.cpp
CXXFLAGS= -O3 --std=c++0x -Wall -Wextra -pedantic -pthread
CXXFLAGS+= -Wl,-z,relro,-z,now,-z,noexecstack -fPIC -pie -fPIE -fstack-protector-all


test.out: main.o
	g++ main.o -O3 --std=c++0x -Wall -Wextra -pedantic -pthread -Wl,-z,relro,-z,now,-z,noexecstack -fPIC -pie -fPIE -fstack-protector-all -o test.out
	
main.o : main.cpp

install:
	# cp main /usr/bin/main

uninstall:
	# rm -f /usr/bin/main

clean:
	rm -f *.o test.out

## eof Makefile