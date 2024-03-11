# Copyright 2019 Google Inc.
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
RUN apt-get update && apt-get install -y build-essential make curl wget libtool cmake ninja-build libbrotli-dev liblzma-dev libz-dev \
    pkg-config zlib1g-dev iptables libc++-dev libc++abi-dev libpthread-stubs0-dev python3-pip


RUN git clone --depth 1 https://github.com/google/libprotobuf-mutator.git 
# proto_mutator
RUN (mkdir LPM && \
    cd LPM && \
    cmake ../libprotobuf-mutator -GNinja -DLIB_PROTO_MUTATOR_DOWNLOAD_PROTOBUF=ON \
    -DLIB_PROTO_MUTATOR_TESTING=OFF -DCMAKE_BUILD_TYPE=Release && \
    ninja)

# Install latest cmake.
RUN wget https://github.com/Kitware/CMake/releases/download/v3.14.5/cmake-3.14.5-Linux-x86_64.sh && \
    chmod +x cmake-3.14.5-Linux-x86_64.sh && \
    ./cmake-3.14.5-Linux-x86_64.sh --skip-license --prefix="/usr/local"

RUN git clone https://github.com/open-source-parsers/jsoncpp
WORKDIR jsoncpp




COPY jsoncpp_proto_bin.cc proto.proto build.sh $SRC/jsoncpp/

