IFLAGS = \
    -I ../../src/include \
    -I ../../src/include/encoder \
    -I ../


test: testencoder 
	./testencoder

testencoder:
	g++ -g -ggdb -Wall -Wextra -std=c++11 -O0 $(IFLAGS) testEncoder.cpp -o testencoder

