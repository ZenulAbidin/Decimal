CXX=g++
CXXFLAGS=-Wall -std=c++11 -g -fPIC -I./include
LDFLAGS=

all: libxFD.so

tests: test_decimal playground

%.o: %.cpp
	${CXX} -c $^ -o $@ ${CXXFLAGS}

libxFD.so: src/Decimal.o
	${CXX} -shared $^ -o $@ ${LDFLAGS}

test_decimal: tests/Decimal.o libxFD.so
	${CXX} $^ -o $@ ${LDFLAGS} -lboost_unit_test_framework

playground: tests/playground.o libxFD.so
	${CXX} $^ -o $@ ${LDFLAGS}

clean:
	rm -f src/*.o tests/*.o libxFD.so test_decimal playground
