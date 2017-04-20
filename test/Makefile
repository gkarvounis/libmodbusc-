IFLAGS = \
    -I ../encoder \
    -I ../server_utils \
    -I ../

.PHONY=test

test: testencoder testserver
	./testencoder
	./testserver

testencoder:
	g++ -g -ggdb -Wall -Wextra -std=c++11 -O0 $(IFLAGS) testEncoder.cpp -o testencoder

testserver:
	g++ -g -ggdb -Wall -Wextra -std=c++11 -O0 $(IFLAGS) testServer.cpp -o testserver -lboost_system


