FROM arm64v8/ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=America/Sao_Paulo

RUN apt-get update && apt-get install -y \
    gawk wget git diffstat unzip texinfo gcc build-essential \
    chrpath socat cpio python3 python3-pip python3-pexpect \
    xz-utils debianutils iputils-ping python3-git python3-jinja2 \
    libegl1-mesa libsdl1.2-dev pylint xterm python3-subunit \
    mesa-common-dev zstd liblz4-tool locales sudo tmux nano curl

RUN locale-gen en_US.UTF-8
ENV LANG=en_US.UTF-8

RUN useradd -m -s /bin/bash yoctouser && \
    echo "yoctouser ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/yoctouser

USER yoctouser
WORKDIR /home/yoctouser

CMD ["/bin/bash"]
