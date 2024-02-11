# Copyright 2020 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
################################################################################

FROM gcr.io/oss-fuzz-base/base-builder@sha256:87ca1e9e19235e731fac8de8d1892ebe8d55caf18e7aa131346fc582a2034fdd

RUN apt-get update &&  \
    apt-get upgrade -y && \
    apt-get install -y \
    make \
    autoconf \
    libtool \
    wget \
    libtool cmake ninja-build libbrotli-dev liblzma-dev libz-dev \
    pkg-config zlib1g-dev iptables libc++-dev libc++abi-dev libpthread-stubs0-dev meson

RUN git clone git://git.sv.nongnu.org/freetype/freetype2.git

# RUN (cd freetype2 && meson build && cd build && ninja && cd ..)


RUN git clone https://github.com/unicode-org/text-rendering-tests.git TRT
RUN wget https://github.com/libarchive/libarchive/releases/download/v3.4.3/libarchive-3.4.3.tar.xz

RUN git clone --depth 1 https://github.com/google/libprotobuf-mutator.git 
# proto_mutator
RUN (mkdir LPM && \
    cd LPM && \
    cmake ../libprotobuf-mutator -GNinja -DLIB_PROTO_MUTATOR_DOWNLOAD_PROTOBUF=ON \
    -DLIB_PROTO_MUTATOR_TESTING=OFF -DCMAKE_BUILD_TYPE=Release && \
    ninja)

COPY * $SRC/
