#!/bin/bash -ex

# Build project.
cd lcms
./autogen.sh && ./configure && make -j
cd ..

export FUZZ_TARGET=cms_transform_fuzzer_proto_bin.cc
export PROTO_FILE=lcms.proto

FILE2PROTO_CONVERTER=FILE2SPEED_BIN.CC
