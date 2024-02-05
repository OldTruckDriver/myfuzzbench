#!/bin/bash -ex

# Build project.
./autogen.sh && ./configure && make -j

# Build fuzz target in $OUT directory.
export FUZZ_TARGET=cms_transform_fuzzer_proto_bin

$CC $CFLAGS -c -Ilcms/include \
        $SRC/$FUZZ_TARGET.c -o $SRC/$FUZZ_TARGET.o
$CXX $CXXFLAGS \
        $SRC/$FUZZ_TARGET.o -o $OUT/$FUZZ_TARGET \
        $LIB_FUZZING_ENGINE lcms/src/.libs/liblcms2.a

cp -r seeds $OUT/

# Optional: Copy dictionary to $OUT directory.
cp $FUZZ_TARGET.dict $OUT/