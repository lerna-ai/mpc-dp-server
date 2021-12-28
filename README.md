# Lerna mpc
> Lerna noise API


## Table of contents

- [Usage](#Usage)
  - [Compile](#Compile)
  - [Prerequisites](#prerequisites)
  - [Running the application](#Run)
  - [Run in Docker](#run-in-docker)
    - [Build the Docker image](#build-the-docker-image)
    - [Run the Docker container](#run-the-docker-container)

## Usage

### Compile

For compiling the mpc server use the make utility.

```bash
# compile project
#
make
```

Note: needs to include libxml2 path when compiling, -lcrypto -lssl -pthread -lxml2 when linking, and run with port number.

### Prerequisites

To run Lerna mpc we need to provide valid certificate files under the following paths

```bash
# Certificate file
./myCA/cacert.pem

# Private key file
./myCA/private/cakey.pem
```

### Run

Lerna mpc is a stand alone application, in order to run application use the following command.

```bash
# Run the server
#
./noiseapi
```

### Run in Docker

#### Build the Docker image

To build image, change to the project directory and run the `docker build` command, optionally specifying a tag. For example, to build the Docker image and tag it as version 1.0.0, do:

```bash
# Build Docker image
#
docker build . -t mpc:1.0.0
```

#### Run the Docker container

To start the Docker container with application, execute the following command:

```bash
# Run Docker container and publish 31337 port
#
docker run --rm -it -p 31337:31337 mpc:1.0.0
```
