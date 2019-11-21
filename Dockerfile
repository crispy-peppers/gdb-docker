#FROM ubuntu
FROM kalilinux/kali-linux-docker
# Metadata params
ARG BUILD_DATE
ARG VERSION
ARG VCS_URL
ARG VCS_REF

LABEL org.label-schema.build-date=$BUILD_DATE \
      org.label-schema.vcs-url=$VCS_URL \
      org.label-schema.vcs-ref=$VCS_REF \
      org.label-schema.version=$VERSION \
      org.label-schema.name='Kali Linux' \
      org.label-schema.description='Official Kali Linux docker image' \
      org.label-schema.usage='https://www.kali.org/news/official-kali-linux-docker-images/' \
      org.label-schema.url='https://www.kali.org/' \
      org.label-schema.vendor='Offensive Security' \
      org.label-schema.schema-version='1.0' \
      org.label-schema.docker.cmd='docker run --rm kalilinux/kali-linux-docker' \
      org.label-schema.docker.cmd.devel='docker run --rm -ti kalilinux/kali-linux-docker' \
      org.label-schema.docker.debug='docker logs $CONTAINER' \
      io.github.offensive-security.docker.dockerfile="Dockerfile" \
      io.github.offensive-security.license="GPLv3" \
      MAINTAINER="Steev Klimaszewski <steev@kali.org>"
RUN echo "deb http://http.kali.org/kali kali-rolling main contrib non-free" > /etc/apt/sources.list && \
    echo "deb-src http://http.kali.org/kali kali-rolling main contrib non-free" >> /etc/apt/sources.list
ENV DEBIAN_FRONTEND noninteractive
RUN set -x \
    && apt-get -yqq update \
    && apt-get -yqq dist-upgrade \
    && apt-get clean
CMD ["bash"]
RUN apt-get update && apt-get -y dist-upgrade
RUN apt-get install -y golang git curl kali-linux kali-linux-top10

# install kubectl
RUN curl -LO https://storage.googleapis.com/kubernetes-release/release/$(curl -s https://storage.googleapis.com/kubernetes-release/release/stable.txt)/bin/linux/amd64/kubectl
RUN chmod +x ./kubectl
RUN mv ./kubectl /usr/local/bin/kubectl
RUN alias kc="kubectl"

ENV GOPATH /usr/go
RUN mkdir $GOPATH
ENV PATH $GOPATH/bin:$PATH

RUN go get github.com/yudai/gotty

ENV TERM xterm

COPY entrypoint.sh /entrypoint.sh
RUN chmod 755 /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
