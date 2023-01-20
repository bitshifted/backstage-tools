FROM ubuntu:22.04

RUN apt update && apt install -y nsis rpm hfsprogs

COPY libdmg-hfsplus/build/dmg/dmg /usr/local/bin/dmg

RUN mkdir /var/run/sshd
RUN echo 'root:root' | chpasswd
COPY docker/sshd_config /etc/ssh/sshd_config

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]