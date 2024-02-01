FROM gcr.io/oss-fuzz-base/base-builder@sha256:87ca1e9e19235e731fac8de8d1892ebe8d55caf18e7aa131346fc582a2034fdd

RUN apt-get update && \
    apt-get install -y \
    make \
    automake \
    libtool \
    wget \
    binutils cmake ninja-build liblzma-dev libz-dev pkg-config zlib1g-dev iptables 

RUN git clone https://github.com/mm2/Little-CMS.git lcms


RUN git clone --depth 1 https://github.com/google/libprotobuf-mutator.git 
# proto_mutator
RUN (mkdir LPM && \
    cd LPM && \
    cmake ../libprotobuf-mutator -GNinja -DLIB_PROTO_MUTATOR_DOWNLOAD_PROTOBUF=ON \
    -DLIB_PROTO_MUTATOR_TESTING=OFF -DCMAKE_BUILD_TYPE=Release && \
    ninja)

COPY *.cc *.h lcms.proto build.sh $SRC/
ADD seeds /opt/seeds