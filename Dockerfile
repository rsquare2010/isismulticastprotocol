FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y gcc --fix-missing

ADD ./src /src/
WORKDIR /src/
RUN gcc udp-send1.c -o udp-send1
ENTRYPOINT ["/src/udp-send1"]	
