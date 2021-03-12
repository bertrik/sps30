CXXFLAGS = -W -Wall -O2

all: sps030_test

clean:
	rm -f sps030_test

sps030_test: sps030_test.cpp sps030.cpp

test: sps030_test
	./sps030_test

