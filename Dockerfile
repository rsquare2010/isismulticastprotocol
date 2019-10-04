FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y gcc --fix-missing

ADD ./src /src/
WORKDIR /src/
RUN gcc udp-recv.c -o udp-recv
ENTRYPOINT ["/src/udp-recv"]	
