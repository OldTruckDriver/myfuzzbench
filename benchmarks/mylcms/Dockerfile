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

RUN apt-get update && \
    apt-get install -y \
    make \
    automake \
    libtool \
    wget binutils cmake ninja-build liblzma-dev \
    libz-dev pkg-config zlib1g-dev iptables

RUN git clone https://github.com/mm2/Little-CMS.git
RUN git clone --depth 1 https://github.com/google/libprotobuf-mutator.git
RUN (mkdir LPM && cd LPM && cmake ../libprotobuf-mutator -GNinja -DLIB_PROTO_MUTATOR_DOWNLOAD_PROTOBUF=ON -DLIB_PROTO_MUTATOR_TESTING=OFF -DCMAKE_BUILD_TYPE=Release && ninja)

RUN wget -qO $OUT/cms_transform_fuzzer.dict \
    https://raw.githubusercontent.com/google/fuzzing/master/dictionaries/icc.dict

# RUN mkdir $SRC/seeds && \
#     cd $SRC/seeds && \
#     cp $SRC/lcms/testbed/bad.icc . && \
#     cp $SRC/lcms/testbed/toosmall.icc . && \
#     cp $SRC/lcms/testbed/test1.icc . && \
#     cp $SRC/lcms/testbed/crayons.icc . && \
#     cp $SRC/lcms/testbed/ibm-t61.icc . && \
#     zip -rj $SRC/seed_corpus.zip $SRC/seeds/*
COPY build.sh *.c *.cc *.h lcms.proto *.options *.dict $SRC/
COPY cms_transform_fuzzer_proto_bin.cc build.sh $SRC/
ADD seeds /opt/seeds
