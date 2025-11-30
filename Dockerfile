FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Базовые утилиты и компилятор
RUN apt-get update && apt-get install -y --no-install-recommends \
    software-properties-common git cmake ninja-build gdb clang-format \
    && add-apt-repository -y ppa:ubuntu-toolchain-r/test \
    && apt-get update && apt-get install -y gcc-14 g++-14 \
    && rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 \
 && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100

WORKDIR /workspace