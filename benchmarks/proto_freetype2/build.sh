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


export PROJECT_NAME=freetyp2
export FUZZ_TARGET=ftfuzzer.cc
export PROTO_FILE=freetype2.proto
export FILE2PROTO_CONVERTER=file2speed_bin.cc


rm -rf genfiles && mkdir genfiles && LPM/external.protobuf/bin/protoc $PROJECT_NAME.proto --cpp_out=genfiles
$CXX $CXXFLAGS -c genfiles/$PROJECT_NAME.pb.cc -DNDEBUG -o genfiles/$PROJECT_NAME.pb.o -I $SRC/LPM/external.protobuf/include


#build file2speed
$CXX $CXXFLAGS $FILE2PROTO_CONVERTER freetyp2proto.cc -I. -I$PROJECT_NAME/include -I$PROJECT_NAME/src -I genfiles -ILPM/external.protobuf/include -I libprotobuf-mutator/ genfiles/woff2.pb.o -lz -lm LPM/src/libfuzzer/libprotobuf-mutator-libfuzzer.a LPM/src/libprotobuf-mutator.a -Wl,--start-group LPM/external.protobuf/lib/lib*.a -Wl,--end-group woff2/build/libconvert_woff2ttf_fuzzer.a $FUZZER_LIB -o $OUT/file2pseed_bin -pthread

mkdir $OUT/seeds
# TRT/fonts is the full seed folder, but they're too big
cp TRT/fonts/TestKERNOne.otf $OUT/seeds/
cp TRT/fonts/TestGLYFOne.ttf $OUT/seeds/

tar xf libarchive-3.4.3.tar.xz

cd libarchive-3.4.3
./configure --disable-shared
make clean
make -j $(nproc)
make install
cd ..

cd freetype2
./autogen.sh
./configure --with-harfbuzz=no --with-bzip2=no --with-png=no --without-zlib
make clean
make all -j $(nproc)

$CXX $CXXFLAGS -std=c++11 -I include -I . src/tools/ftfuzzer/ftfuzzer.cc \
    objs/.libs/libfreetype.a $FUZZER_LIB -L /usr/local/lib -larchive \
    -o $OUT/ftfuzzer
