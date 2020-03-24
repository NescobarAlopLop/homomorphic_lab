FROM ubuntu:18.04

# Install binary dependencies
RUN apt-get -qqy update && apt-get install -qqy \
	g++ \
	git \
	make \
	ca-certificates \
	wget \
	libssl-dev \
	--no-install-recommends

RUN apt-get purge cmake
RUN cd /tmp && wget https://github.com/Kitware/CMake/releases/download/v3.17.0/cmake-3.17.0.tar.gz
RUN cd /tmp && tar -xzvf cmake-3.17.0.tar.gz
RUN cd tmp/cmake-3.17.0 && ./bootstrap && make -j4 && make install

# Build SEAL libraries
RUN git clone https://github.com/microsoft/SEAL.git
RUN cd /SEAL && cd native/src/ &&  cmake . && make && make install
RUN cd /SEAL && cd native/examples/ &&  cmake . && make
