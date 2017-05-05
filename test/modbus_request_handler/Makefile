IFLAGS = \
    -I ../../encoder \
    -I ../../server_utils \
    -I ../../ \
    -I ../

test: testModbusRequestHandler
	./testModbusRequestHandler

testModbusRequestHandler:
	g++ -g -ggdb -Wall -Wextra -std=c++11 -O0 $(IFLAGS) testModbusRequestHandler.cpp -o testModbusRequestHandler -lboost_system


