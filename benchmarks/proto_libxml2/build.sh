#!/bin/bash -ex
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

rm -rf $SRC/genfiles && mkdir $SRC/genfiles && $SRC/LPM/external.protobuf/bin/protoc proto.proto --cpp_out=genfiles
$CXX $CXXFLAGS -c genfiles/proto.pb.cc -DNDEBUG -o genfiles/proto.pb.o -I $SRC/LPM/external.protobuf/include



cd libfuzzer && ./build.sh && cd ..

cd libxml2

if [ "$SANITIZER" = undefined ]; then
    export CFLAGS="$CFLAGS -fsanitize=unsigned-integer-overflow -fno-sanitize-recover=unsigned-integer-overflow"
    export CXXFLAGS="$CXXFLAGS -fsanitize=unsigned-integer-overflow -fno-sanitize-recover=unsigned-integer-overflow"
fi

export V=1

./autogen.sh \
    --disable-shared \
    --without-debug \
    --without-ftp \
    --without-http \
    --without-legacy \
    --without-python
make -j$(nproc)

# cd fuzz
# make clean-corpus
# make fuzz.o

# make xml.o
# Link with $CXX
$CXX $CXXFLAGS /src/xml.cc  -std=c++14 -I/src/ -I/src/libxml2/include \
    -I/src/LPM/external.protobuf/include -I/src/libprotobuf-mutator/ -I/src/libxml2/fuzz \
    /src/genfiles/proto.pb.o \
    /src/LPM/src/libfuzzer/libprotobuf-mutator-libfuzzer.a /src/LPM/src/libprotobuf-mutator.a -Wl,--start-group \
    /src/LPM/external.protobuf/lib/lib*.a -Wl,--end-group \
    -o $OUT/xml \
    /src/libfuzzer/libFuzzer.a \
    /src/libxml2/.libs/libxml2.a -Wl,-Bstatic -lz -llzma -Wl,-Bdynamic -lpthread -fsanitize=fuzzer,address

# [ -e seed/xml ] || make seed/xml.stamp
# zip -j $OUT/xml_seed_corpus.zip seed/xml/*

# cp *.dict *.options $OUT/
