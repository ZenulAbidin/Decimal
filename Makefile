CXX=g++
CXXFLAGS=-Wall -std=c++11 -g -fPIC -I./include
LDFLAGS=

all: libdecimal.so

tests: test_decimal playground

%.o: %.cpp
	${CXX} -c $^ -o $@ ${CXXFLAGS}

libdecimal.so: src/Decimal.o
	${CXX} -shared $^ -o $@ ${LDFLAGS}

test_decimal: tests/Decimal.o libdecimal.so
	${CXX} $^ -o $@ ${LDFLAGS} -lboost_unit_test_framework

playground: tests/playground.o libdecimal.so
	${CXX} $^ -o $@ ${LDFLAGS}

clean:
	rm -f src/*.o tests/*.o libdecimal.so test_decimal playground
