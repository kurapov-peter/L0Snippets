FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    git \
    make \
    wget \
    pkg-config \
    libz-dev \
    --

RUN apt-get update 
RUN apt-get install -y llvm-10 llvm-10-dev llvm-10-tools clang-10 libclang-10-dev

# Intel GPU stack
# https://dgpu-docs.intel.com/installation-guides/ubuntu/ubuntu-focal.html
RUN apt update
RUN apt install -y gpg-agent software-properties-common
RUN wget -qO - https://repositories.intel.com/graphics/intel-graphics.key | apt-key add -
RUN apt-add-repository \
    'deb [arch=amd64] https://repositories.intel.com/graphics/ubuntu focal main'
RUN apt-get update
RUN apt-get install -y \
    intel-igc-cm \
    intel-level-zero-gpu \
    intel-media-va-driver-non-free \
    intel-opencl-icd \
    level-zero \
    level-zero-dev \
    libigc-dev \
    libigdfcl-dev \
    libigfxcmrt-dev \
    libmfx1 \
    --

# SPIRV-Translator
# https://github.com/KhronosGroup/SPIRV-LLVM-Translator/tree/llvm_release_100#build-with-pre-installed-llvm
RUN git clone -b llvm_release_100 https://github.com/KhronosGroup/SPIRV-LLVM-Translator.git
RUN mkdir SPIRV-LLVM-Translator/build && cd SPIRV-LLVM-Translator/build && cmake -Wno-dev ..
RUN cd SPIRV-LLVM-Translator/build && make llvm-spirv -j`nproc` && make install
RUN cp SPIRV-LLVM-Translator/build/tools/llvm-spirv/llvm-spirv /usr/bin/

RUN apt-get clean

RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-10 100
RUN update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-10 100

