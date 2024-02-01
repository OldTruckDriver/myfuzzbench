#!/bin/bash -ex

# Build project.
cd lcms
./autogen.sh && ./configure && make -j
cd ..

export FUZZ_TARGET=cms_transform_fuzzer_proto_bin.cc
export PROTO_FILE=lcms.proto

FILE2PROTO_CONVERTER=FILE2SPEED_BIN.CC

rm -rf genfiles && midir genfiles && LPM/external.protobuf/bin/protoc lcms.proto --cpp_out=genfiles
$CXX $CXXFLAGS -c genfiles/lcms.pb.cc -DNDEBUG -o genfiles/lcms.pb.o -I $SRC/external.protobuf/include

#build file2speed
$CXX $CXXFLAGS  $FILE2PROTO_CONVERTER lcms2proto.cc -I. -Ilcms/include -Ilcms/src \
                -I genfiles -ILPM/external.protobuf/include  -I libprotobuf-mutator/ \
                genfiles/lcms.pb.o  \
                -lz -lm \
                LPM/src/libfuzzer/libprotobuf-mutator-libfuzzer.a \
                LPM/src/libprotobuf-mutator.a \
                -Wl,--start-group LPM/external.protobuf/lib/lib*.a -Wl,--end-group \
                lcms/src/.libs/liblcms2.a \
                $FUZZER_LIB \
                -o $OUT/file2pseed_bin

# build fuzzer
$CXX $CXXFLAGS  $FUZZ_TARGET proto2lcms.cc -I. -Ilcms/include -Ilcms/src -Ilcms/include -Ilcms/src \
                -I genfiles -ILPM/external.protobuf/include  -I libprotobuf-mutator/ \
                genfiles/lcms.pb.o  \
                -lz -lm \
                LPM/src/libfuzzer/libprotobuf-mutator-libfuzzer.a \
                LPM/src/libprotobuf-mutator.a \
                -Wl,--start-group LPM/external.protobuf/lib/lib*.a -Wl,--end-group \
                lcms/src/.libs/liblcms2.a \
                $FUZZER_LIB \
                -o $OUT/cms_transform_fuzzer_proto_bin
mkdir $OUT/seeds
for file in /opt/seeds/*.icc; do
    filename=$(basename "$file")
    extension="${filename##*.}"
    filename="${filename%.*}"

    output_file="${OUT}/seeds/${filename}.pbbin"
    $OUT/file2pseed_bin "$file" "$output_file"
done