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

cd $SRC
rm -rf genfiles && mkdir genfiles && LPM/external.protobuf/bin/protoc proto.proto --cpp_out=genfiles
$CXX $CXXFLAGS -c /src/genfiles/proto.pb.cc -DNDEBUG -o $SRC/genfiles/proto.pb.o -I/src/LPM/external.protobuf/include

cd $SRC/libfuzzer && ./build.sh && cd $SRC/json-c


mkdir json-c-build
cd json-c-build
cmake -DBUILD_SHARED_LIBS=OFF ..
make -j$(nproc)
cd ..

cp $SRC/*.dict $OUT/

$CXX $CXXFLAGS -std=c++14 $SRC/tokener_parse_ex_fuzzer.cc \
         -I$SRC/genfiles -I$SRC/json-c -I$SRC/json-c/json-c-build \
         -o $OUT/tokener_parse_ex_fuzzer \
         -I/src/LPM/external.protobuf/include -I/src/libprotobuf-mutator/ -I/src/ \
         /src/genfiles/proto.pb.o \
         /src/LPM/src/libfuzzer/libprotobuf-mutator-libfuzzer.a \
         /src/LPM/src/libprotobuf-mutator.a -Wl,--start-group \
         /src/LPM/external.protobuf/lib/lib*.a -Wl,--end-group \
         /src/libfuzzer/libFuzzer.a \
         -Wl,-Bstatic -lz -llzma -Wl,-Bdynamic -lpthread -fsanitize=fuzzer,address \
         $SRC/json-c/json-c-build/libjson-c.a




# for f in $SRC/*_fuzzer.cc; do
#     fuzzer=$(basename "$f" _fuzzer.cc)
#     $CXX $CXXFLAGS -std=c++11 -I$SRC/json-c -I$SRC/json-c/json-c-build\
#          $SRC/${fuzzer}_fuzzer.cc -o $OUT/${fuzzer}_fuzzer \
#          -I/src/LPM/external.protobuf/include -I/src/libprotobuf-mutator/ -I/src/ \
#          /src/genfiles/proto.pb.o \
#          /src/LPM/src/libfuzzer/libprotobuf-mutator-libfuzzer.a \
#          /src/LPM/src/libprotobuf-mutator.a -Wl,--start-group \
#          /src/LPM/external.protobuf/lib/lib*.a -Wl,--end-group \
#          /src/libfuzzer/libFuzzer.a \
#          -Wl,-Bstatic -lz -llzma -Wl,-Bdynamic -lpthread -fsanitize=fuzzer,address \
#          $SRC/json-c/json-c-build/libjson-c.a
# done

