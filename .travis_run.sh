cd .travis_docker
 docker build -t libmodbus/build:latest .
docker run --rm -v $TRAVIS_BUILD_DIR:/root/libmodbusc++ libmodbus/build:latest

