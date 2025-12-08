ARG REGISTRY
ARG BASE_IMAGE_TAG=latest

FROM ${REGISTRY}kvcpp-dev:${BASE_IMAGE_TAG} AS build
ARG DEBIAN_FRONTEND=noninteractive
RUN apt update && apt install -y gfortran gpg software-properties-common apt-utils cmake \
  libgsl-dev libsqlite3-dev python-is-python3

COPY CMakeLists.txt Doxyfile format-logs.pl kvqc2d.1 \
  plu-autoupdate-from-stinfosys.in plu-update-from-stinfosys \
  README version.sh debian-version.sh \
  /src/

COPY data /src/data
COPY doc /src/doc
COPY debian /src/debian
COPY src /src/src
COPY test /src/test

VOLUME /src
VOLUME /build
WORKDIR /build


RUN cmake -DCMAKE_INSTALL_PREFIX=/usr /src && \
  make -j4 && \
  make install 

ENTRYPOINT [ "/usr/bin/bash" ]


FROM ${REGISTRY}kvcpp-runtime:${BASE_IMAGE_TAG} 
ARG DEBIAN_FRONTEND=noninteractive
ARG kvuser=kvalobs
ARG kvuserid=5010

RUN apt update && apt install -y libsqlite3-0 libgsl27  libgslcblas0 python-is-python3

#Create a runtime user for kvalobs
RUN addgroup --gid $kvuserid $kvuser && \
  adduser --uid $kvuserid  --gid $kvuserid --disabled-password --disabled-login --gecos '' $kvuser

RUN mkdir -p /etc/kvalobs && chown ${kvuser}:${kvuser}  /etc/kvalobs
RUN mkdir -p /var/log/kvalobs && chown ${kvuser}:${kvuser}  /var/log/kvalobs
RUN mkdir -p /var/lib/kvalobs/run && chown ${kvuser}:${kvuser} /var/lib/kvalobs/run

COPY --from=build /usr/bin/kvqc2d /usr/bin/kvqc2d
VOLUME /var/log/kvalobs
VOLUME /etc/kvalobs

USER ${kvuser}:${kvuser}

ENTRYPOINT ["kvqc2d"]
