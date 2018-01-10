FROM ubuntu:latest
RUN apt-get -y update && apt-get -y upgrade && apt-get -y install g++ libboost-all-dev make cmake
CMD cd /root/libmodbusc++/ && \
    mkdir build && cd build && cmake ../ && \
    make && \
    test/encoder/testencoder && \
    test/decoder/testdecoder && \
    test/serverDevice/test_server_device && \
    test/server/test_server && \
    test/modbuscli/testmodbuscli && \
    test/socket_connector/testSocketConnector && \
    test/modbus_poller/testModbusPoller

