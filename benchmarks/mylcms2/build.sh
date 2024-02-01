#!/bin/bash -eu
# Copyright 2016 Google Inc.
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

# build the target.
(cd lcms/ && ./configure --enable-shared=no && make -j$(nproc) all && cd ..)

# myproto
# build protobuf
rm -rf genfiles && mkdir genfiles && LPM/external.protobuf/bin/protoc lcms.proto --cpp_out=genfiles




# build your fuzzer(s)
export FUZZ_TARGET="cms_transform_fuzzer_proto_bin"
# FUZZERS="cms_transform_fuzzer_proto_bin"


$CC $CFLAGS -c -Ilcms/include -I/src/lcms/src -I/src/genfiles -I/src/libprotobuf-mutator -I/src/LPM/external.protobuf/include \
    $SRC/$FUZZ_TARGET.cc -o $SRC/$FUZZ_TARGET.o \
                -lprotobuf 
$CXX $CXXFLAGS \
    $SRC/$FUZZ_TARGET.o -o $OUT/$FUZZ_TARGET \
    lcms/src/.libs/liblcms2.a \
                -lprotobuf 

echo "my_variable: $LIB_FUZZING_ENGINE"

# $CXX $CXXFLAGS $SRC/cms_transform_fuzzer_proto_bin.cc \
#     -I lcms/include lcms/src/.libs/liblcms2.a \
#     $LIB_FUZZING_ENGINE \
#     -o $OUT/$FUZZ_TARGET

# for F in $FUZZERS; do
#     $CC $CFLAGS -c -Ilcms/include \
#         $SRC/$F.cc -o $SRC/$F.o
#     $CXX $CXXFLAGS \
#         $SRC/$F.o -o $OUT/$F \
#         $LIB_FUZZING_ENGINE lcms/src/.libs/liblcms2.a
# done


# protofuzzer
$CXX $CXXFLAGS  -c -Ilcms/include -Ilcms/src -I genfiles -I.  -I LPM/external.protobuf/include lcms2proto.cc  -o lcms2proto.o
$CXX $CXXFLAGS  -c -Ilcms/include -Ilcms/src -I genfiles -I.  -I LPM/external.protobuf/include proto2lcms.cc  -o proto2lcms.o
$CXX $CXXFLAGS  -Ilcms/include -Ilcms/src -I genfiles -I.  -I LPM/external.protobuf/include\
                file2pseed_bin.cc genfiles/lcms.pb.cc lcms2proto.o \
                -fuse-ld=lld -lz \
                lcms/src/.libs/liblcms2.a \
                LPM/external.protobuf/lib/lib*.a \
                LPM/src/libfuzzer/libprotobuf-mutator-libfuzzer.a \
                LPM/src/libprotobuf-mutator.a \
                -o $OUT/file2pseed_bin \
                -lprotobuf 

$CXX $CXXFLAGS  -Ilcms/include -Ilcms/src -I genfiles -I.  -I LPM/external.protobuf/include\
                file2pseed_text.cc genfiles/lcms.pb.cc lcms2proto.o \
                -fuse-ld=lld -lz \
                lcms/src/.libs/liblcms2.a \
                LPM/external.protobuf/lib/lib*.a \
                LPM/src/libfuzzer/libprotobuf-mutator-libfuzzer.a \
                LPM/src/libprotobuf-mutator.a \
                -o $OUT/file2pseed_text \
                -lprotobuf 

$CXX $CXXFLAGS  -DNDEBUG -Ilcms/include -Ilcms/src -I genfiles -I.  -I libprotobuf-mutator/  -I LPM/external.protobuf/include \
                cms_transform_fuzzer_proto_text.cc genfiles/lcms.pb.cc lcms2proto.o  proto2lcms.o \
                -fuse-ld=lld -lz \
                lcms/src/.libs/liblcms2.a \
                $LIB_FUZZING_ENGINE \
                LPM/external.protobuf/lib/lib*.a \
                LPM/src/libfuzzer/libprotobuf-mutator-libfuzzer.a \
                LPM/src/libprotobuf-mutator.a \
                -o $OUT/cms_transform_fuzzer_proto_text \
                -lprotobuf 

$CXX $CXXFLAGS  -DNDEBUG -Ilcms/include -Ilcms/src -I genfiles -I.  -I libprotobuf-mutator/  -I LPM/external.protobuf/include \
                cms_transform_fuzzer_proto_bin.cc genfiles/lcms.pb.cc lcms2proto.o  proto2lcms.o \
                -fuse-ld=lld -lz \
                lcms/src/.libs/liblcms2.a \
                $LIB_FUZZING_ENGINE \
                LPM/external.protobuf/lib/lib*.a \
                LPM/src/libfuzzer/libprotobuf-mutator-libfuzzer.a \
                LPM/src/libprotobuf-mutator.a \
                -o $OUT/cms_transform_fuzzer_proto_bin \
                -lprotobuf 

cp $SRC/icc.dict $SRC/*.options $OUT/
cp $SRC/icc.dict $OUT/cms_transform_all_fuzzer.dict
cp $SRC/icc.dict $OUT/cms_transform_extended_fuzzer.dict
cp $SRC/icc.dict $OUT/cms_universal_transform_fuzzer.dict
cp $SRC/icc.dict $OUT/cms_profile_fuzzer.dict

rm -rf proto_seeds_bin && mkdir proto_seeds_bin
for file in seeds/*.icc; do
    filename=$(basename "$file")
    extension="${filename##*.}"
    filename="${filename%.*}"

    output_file="proto_seeds_bin/${filename}.pbbin"
    $OUT/file2pseed_bin "$file" "$output_file"
done

rm -rf proto_seeds_text && mkdir proto_seeds_text
for file in seeds/*.icc; do
    filename=$(basename "$file")
    extension="${filename##*.}"
    filename="${filename%.*}"

    output_file="proto_seeds_text/${filename}.pbtxt"
    $OUT/file2pseed_text "$file" "$output_file"
done


zip -rj $SRC/proto_seed_bin_corpus.zip proto_seeds_bin/*
zip -rj $SRC/proto_seed_text_corpus.zip proto_seeds_text/*
cp $SRC/proto_seed_bin_corpus.zip $OUT/cms_transform_fuzzer_proto_bin_seed_corpus.zip
cp $SRC/proto_seed_text_corpus.zip $OUT/cms_transform_fuzzer_proto_text_seed_corpus.zip
cp $SRC/seed_corpus.zip $OUT/cms_transform_fuzzer_seed_corpus.zip
cp $SRC/seed_corpus.zip $OUT/cms_profile_fuzzer_seed_corpus.zip
cp $SRC/seed_corpus.zip $OUT/cms_universal_transform_fuzzer_seed_corpus.zip
