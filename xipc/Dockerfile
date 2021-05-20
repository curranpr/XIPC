FROM ubuntu:latest
COPY xipc.tar.gz /tmp
WORKDIR /tmp/bin
RUN tar xvf /tmp/xipc.tar.gz -C /tmp
ENV XIPCROOT=/tmp
ENV PATH=$PATH:$XIPCROOT/bin
ENV XIPCSERV=4001
ENV XIPCETC=4000
ENV XIPC=$XIPCROOT/samples/bin/test
COPY dockerXIPC.sh /tmp/bin
COPY Collector3Program.jar /tmp/bin
ENTRYPOINT ["dockerXIPC.sh"]