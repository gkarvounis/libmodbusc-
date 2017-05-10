FROM ubuntu:latest
RUN apt-get -y update && apt-get -y upgrade && apt-get -y install g++ libboost-all-dev make
CMD cd /root/libmodbusc++/test && make
