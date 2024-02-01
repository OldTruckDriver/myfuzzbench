FROM gcr.io/oss-fuzz-base/base-builder

RUN apt-get update && \
    apt-get install -y \
    make automake libtool \
    wget autoconf binutils cmake ninja-build liblzma-dev \
    libz-dev pkg-config zlib1g-dev

RUN git clone https://github.com/mm2/Little-CMS.git


RUN wget -qO $OUT/cms_transform_fuzzer.dict \
    https://raw.githubusercontent.com/google/fuzzing/master/dictionaries/icc.dict

COPY build.sh cms_transform_fuzzer.dict $SRC/
ADD seeds $SRC/seeds