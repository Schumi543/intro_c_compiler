# syntax = docker/dockerfile:experimental
FROM rhub/ubuntu-gcc
RUN apt-get install -y --no-install-recommends gdb git ssh

RUN mkdir /root/.ssh/
RUN touch /root/.ssh/known_hosts
RUN ssh-keyscan -t rsa github.com >> /root/.ssh/known_hosts
RUN --mount=type=secret,id=ssh,dst=/root/.ssh/id_rsa git clone git@github.com:Schumi543/intro_c_compiler.git
