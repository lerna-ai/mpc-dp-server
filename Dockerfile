FROM gcc:9.2 AS build

RUN apt-get update \
    && apt-get install -y --no-install-recommends libssl-dev libxml2 libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src

COPY Makefile main.cpp ./
COPY nbproject/Makefile-impl.mk ./nbproject/
COPY nbproject/Makefile-variables.mk ./nbproject/
COPY nbproject/Makefile-Debug.mk ./nbproject/

RUN make

FROM ubuntu:bionic

RUN apt-get update \
    && apt-get install -y --no-install-recommends libssl-dev libxml2 libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/mpc

EXPOSE 31337

COPY --from=build /src/dist/Debug/GNU-Linux/mpc ./
COPY myCA/cacert.pem ./myCA/cacert.pem
COPY myCA/private/cakey.pem ./myCA/private/cakey.pem

CMD ["./mpc"]

# docker build . -t mpc:1.0.0
# docker run --rm -it -p 31337:31337 mpc:1.0.0
