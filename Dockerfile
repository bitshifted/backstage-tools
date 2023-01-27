FROM ubuntu:22.04

RUN apt update && apt install -y nsis rpm hfsprogs openssh-server sudo

COPY libdmg-hfsplus/build/dmg/dmg /usr/local/bin/dmg
RUN chmod 755 /usr/local/bin/dmg

RUN mkdir /var/run/sshd
RUN echo 'root:root' | chpasswd
COPY docker/sshd_config /etc/ssh/sshd_config

COPY docker/build-nsis.sh /usr/local/bin/build-nsis
RUN chmod 755 /usr/local/bin/build-nsis
COPY docker/build-deb.sh /usr/local/bin/build-deb
RUN chmod 744 /usr/local/bin/build-deb
COPY docker/build-rpm.sh /usr/local/bin/build-rpm
RUN chmod 755 /usr/local/bin/build-rpm
COPY docker/build-dmg.sh /usr/local/bin/build-dmg
RUN chmod 755 /usr/local/bin/build-dmg

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]
