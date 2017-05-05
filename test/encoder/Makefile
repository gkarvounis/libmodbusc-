IFLAGS = \
    -I ../../encoder \
    -I ../../ \
    -I ../


test: testencoder 
	./testencoder

testencoder:
	g++ -g -ggdb -Wall -Wextra -std=c++11 -O0 $(IFLAGS) testEncoder.cpp -o testencoder

