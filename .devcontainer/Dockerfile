# [Choice] Ubuntu version (use jammy or bionic on local arm64/Apple Silicon): jammy, focal, bionic
ARG VARIANT="jammy"
FROM buildpack-deps:${VARIANT}-curl

LABEL dev.containers.features="common"

# [Optional] Uncomment this section to install additional OS packages.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
     && apt-get -y install --no-install-recommends \
        build-essential \
        pkg-config \
        libgnutls28-dev \
        libjansson-dev \
        zlib1g-dev \
        libcurl4-gnutls-dev \
        libmicrohttpd-dev \
        check \
        gnutls-bin
RUN cd /tmp && wget \
   https://github.com/babelouest/orcania/releases/download/v2.3.2/liborcania-dev_2.3.2_ubuntu_jammy_x86_64.deb \
   https://github.com/babelouest/yder/releases/download/v1.4.19/libyder-dev_1.4.19_ubuntu_jammy_x86_64.deb \
   https://github.com/babelouest/ulfius/releases/download/v2.7.12/libulfius-dev_2.7.12_ubuntu_jammy_x86_64.deb \
   && \
   dpkg -i *.deb

