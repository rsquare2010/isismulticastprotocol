FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y gcc --fix-missing

ADD ./src /src/
WORKDIR /src/
RUN gcc udp-send.c -o udp-send
ENTRYPOINT ["/src/udp-send"]	
