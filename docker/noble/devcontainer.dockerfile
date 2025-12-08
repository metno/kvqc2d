ARG REGISTRY="registry.met.no/met/obsklim/bakkeobservasjoner/data-og-kvalitet/kvalobs/kvbuild/staging/"
ARG BASE_IMAGE_TAG=latest

FROM ${REGISTRY}kvcpp-dev:${BASE_IMAGE_TAG}
ARG DEBIAN_FRONTEND='noninteractive'
ARG USER=vscode

RUN apt-get update && \
  apt-get install -y libgmock-dev language-pack-nb-base clang gdb libgeo-bufr-perl \
  metno-bufrtables less mg cmake libgsl-dev libsqlite3-dev python-is-python3

RUN locale-gen en_US.UTF-8
RUN useradd -ms /bin/bash ${USER}

ENV USER=${USER}

ENTRYPOINT [ "/bin/bash" ]