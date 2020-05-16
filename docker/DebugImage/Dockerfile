FROM ubuntu:18.04

# Install binary dependencies
RUN apt-get -qqy update && apt-get install -qqy \
	g++ \
	git \
	ca-certificates \
	wget \
	libssl-dev \
	--no-install-recommends

RUN apt-get update && apt-get install -y \
    ssh \
    build-essential \
    gcc \
    g++ \
    gdb \
    apt-utils \
    openssh-server \
    clang \
    rsync \
    tar \
    python \
    gdb \
    gdbserver \
  && apt-get clean

RUN apt-get purge cmake
RUN cd /tmp && wget https://github.com/Kitware/CMake/releases/download/v3.17.0/cmake-3.17.0.tar.gz
RUN cd /tmp && tar -xzvf cmake-3.17.0.tar.gz
RUN cd tmp/cmake-3.17.0 && ./bootstrap && make -j4 && make install



RUN mkdir /var/run/sshd
RUN echo 'root:root' | chpasswd
RUN sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# SSH login fix. Otherwise user is kicked off after login
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile

# 22 for ssh server. 7777 for gdb server.
EXPOSE 22 7777

RUN useradd -ms /bin/bash debugger
RUN echo 'debugger:pwd' | chpasswd


# Build SEAL libraries
RUN git clone https://github.com/microsoft/SEAL.git SEAL
RUN cd /SEAL  &&  cmake . && make && make install

CMD ["/usr/sbin/sshd", "-D"]
