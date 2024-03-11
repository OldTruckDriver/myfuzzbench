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

rm -rf genfiles && mkdir genfiles && $SRC/LPM/external.protobuf/bin/protoc proto.proto --cpp_out=genfiles
$CXX $CXXFLAGS -c genfiles/proto.pb.cc -DNDEBUG -o genfiles/proto.pb.o -I $SRC/LPM/external.protobuf/include



mkdir -p build
cd build
cmake -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_CXX_FLAGS="$CXXFLAGS" \
      -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF -DJSONCPP_WITH_TESTS=OFF \
      -DBUILD_SHARED_LIBS=OFF -G "Unix Makefiles" ..
make

# Compile fuzzer.
$CXX $CXXFLAGS -std=c++14 -I../.. -I../include -I ../genfiles -I ../../LPM/external.protobuf/include -I ../src/test_lib_json/ -I ../../libprotobuf-mutator/ -I../genfiles/proto.pb.o \
                -I../../LPM/src/libfuzzer/libprotobuf-mutator-libfuzzer.a -I ../../LPM/src/libprotobuf-mutator.a lib/libjsoncpp.a ../../LPM/external.protobuf/lib/lib*.a \
                -lz -lm -lpthread -L./lib -ljsoncpp \
                  -L../../LPM/external.protobuf/lib -lprotobuf \
                  -L../../abseil-cpp/build/lib -labsl_base -labsl_log_internal_globals -labsl_log_internal_format \
                  -labsl_log_internal_check_op -labsl_log_internal_conditions -labsl_log_internal_message \
                  -labsl_log_internal_nullguard -labsl_log_internal_proto -labsl_log_severity -labsl_strings -labsl_strings_internal \
                  -fsanitize=fuzzer,address -Wl,--end-group \
                ../../libfuzzer/libFuzzer.a ../jsoncpp_proto_bin.cc -o $OUT/jsoncpp_fuzzer



# Add dictionary.
cp $SRC/jsoncpp/src/test_lib_json/fuzz.dict $OUT/jsoncpp_fuzzer.dict


$CXX $CXXFLAGS -std=c++14 -I../include -I../genfiles -I../../LPM/external.protobuf/include \
     -I../src/test_lib_json -I../../libprotobuf-mutator -I../genfiles/proto.pb.o -I../../LPM/src/libfuzzer/libprotobuf-mutator-libfuzzer.a -I ../../LPM/src/libprotobuf-mutator.a lib/libjsoncpp.a ../../LPM/external.protobuf/lib/lib*.a\
     -lz -lm -lpthread -L./lib -ljsoncpp \
     -L../../LPM/external.protobuf/lib -lprotobuf \
     -L../../abseil-cpp/build/lib -labsl_base -labsl_log_internal_globals -labsl_log_internal_format \
     -labsl_log_internal_check_op -labsl_log_internal_conditions -labsl_log_internal_message \
     -labsl_log_internal_nullguard -labsl_log_internal_proto -labsl_log_severity -labsl_strings -labsl_strings_internal \
     -fsanitize=fuzzer,address \
     lib/libjsoncpp.a \
     ../../LPM/external.protobuf/lib/lib*.a \
     ../../libfuzzer/libFuzzer.a \
     ../jsoncpp_proto_bin.cc \
     -o $OUT/jsoncpp_fuzzer