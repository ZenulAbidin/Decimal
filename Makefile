CXX=g++
CXXFLAGS=-Wall -std=c++11 -fPIC -I./include
if debug
	CXXFLAGS=${CXXFLAGS} -g -fprofile-arcs -ftest-coverage
else
	CXXFLAGS=${CXXFLAGS} -O2 -fstack-reuse=none -Wstack-protector -fstack-protector-all -fcf-protection=full -fstack-clash-protection
fi

LDFLAGS=

all: libxFD.so

tests: test_decimal playground hexdec

%.o: %.cpp
	${CXX} -c $^ -o $@ ${CXXFLAGS}

libxFD.so: src/Decimal.o
	${CXX} -shared $^ -o $@ ${LDFLAGS}

test_decimal: tests/Decimal.o libxFD.so
	${CXX} $^ -o $@ ${LDFLAGS} -lboost_unit_test_framework

hexdec: tests/hexdec.o libxFD.so
	${CXX} $^ -o $@ ${LDFLAGS}


playground: tests/playground.o libxFD.so
	${CXX} $^ -o $@ ${LDFLAGS}

clean:
	rm -f src/*.o tests/*.o libxFD.so test_decimal hexdec playground
