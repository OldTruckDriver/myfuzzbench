#!/bin/bash -eu
# Copyright 2018 Google Inc.
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

cd ..
cd libfuzzer
./build.sh
cd ..
cd jsoncpp

# git clone https://github.com/abseil/abseil-cpp.git
# cd abseil-cpp
# git checkout lts_2023_08_02
# cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
# make -j$(nproc) && make install
# cd ..


rm -rf genfiles && mkdir genfiles && /src/LPM/external.protobuf/bin/protoc proto.proto --cpp_out=genfiles
$CXX $CXXFLAGS -c genfiles/proto.pb.cc -DNDEBUG -o genfiles/proto.pb.o -I/src/LPM/external.protobuf/include

# rm -rf genfiles && mkdir genfiles && protoc freetype2.proto --cpp_out=genfiles
# $CXX $CXXFLAGS -c genfiles/proto.pb.cc -DNDEBUG -o genfiles/proto.pb.o -I $SRC/LPM/external.protobuf/include

mkdir -p build
cd build
cmake -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_CXX_FLAGS="$CXXFLAGS" \
      -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF -DJSONCPP_WITH_TESTS=OFF \
      -DBUILD_SHARED_LIBS=OFF -G "Unix Makefiles" ..
make


$CXX $CXXFLAGS ../jsoncpp_proto_bin.cc -I../src/test_lib_json -I/src/LPM/external.protobuf/include \
-I/src/ -I/src/libprotobuf-mutator/ -I/src/LPM/ -I/src/jsoncpp/build/protobuf/src/ lib/libjsoncpp.a \
-L/src/LPM/external.protobuf/lib -lprotobuf -fsanitize=fuzzer,address -o $OUT/jsoncpp_fuzzer

#$CXX $CXXFLAGS ../jsoncpp_proto_bin.cc -std=c++14 -I../include/json -I../src/test_lib_json -I../genfiles -I/src/LPM/external.protobuf/include \
#      -I/src/ -I/src/libprotobuf-mutator \
#      -lpthread \
#      ../genfiles/proto.pb.o /src/libfuzzer/libFuzzer.a /src/LPM/src/libfuzzer/libprotobuf-mutator-libfuzzer.a \
#      /src/LPM/external.protobuf/lib/lib*.a \
#      /src/LPM/src/libprotobuf-mutator.a lib/libjsoncpp.a\
#      -o $OUT/jsoncpp_fuzzer



# Add dictionary.
cp $SRC/jsoncpp/src/test_lib_json/fuzz.dict $OUT/jsoncpp_fuzzer.dict









