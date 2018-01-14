FROM ubuntu:latest
RUN apt-get -y update && apt-get -y upgrade && apt-get -y install g++ libboost-all-dev make cmake
CMD cd /root/libmodbusc++/ && \
    mkdir build && cd build && cmake ../ && \
    make && \
    test/encoder/testencoder -d yes && \
    test/decoder/testdecoder -d yes && \
    test/serverDevice/test_server_device -d yes && \
    test/server/test_server -d yes && \
    test/modbuscli/testmodbuscli -d yes && \
    test/socket_connector/testSocketConnector -d yes && \
    test/modbus_poller/testModbusPoller -d yes

