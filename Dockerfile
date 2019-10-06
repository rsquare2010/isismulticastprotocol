FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y g++ --fix-missing

ADD ./src /src/
WORKDIR /src/
RUN g++ udp-send1.cpp -o udp-send1
ENTRYPOINT ["/src/udp-send1"]	
