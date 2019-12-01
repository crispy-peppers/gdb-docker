FROM ubuntu:18.04
RUN apt-get update && apt-get -y install gcc-multilib gdb python3 
RUN apt-get update && apt-get -y install procps net-tools less vim mlocate sudo nano wget curl
COPY vuln-echo-server.c /
RUN cp /vuln-echo-server.c /tmp
RUN gcc vuln-echo-server.c -g -o vuln-echo-server -m32 -fno-stack-protector -z execstack -no-pie -Wl,-z,norelro
EXPOSE 80
EXPOSE 8080
COPY start.sh /
RUN chmod +x /start.sh
ENTRYPOINT ["/start.sh"]