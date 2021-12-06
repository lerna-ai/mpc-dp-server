# Lerna mpc
> Lerna noise API


## Table of contents

- [Usage](#Usage)
  - [Compile](#Compile)
  - [Prerequisites](#prerequisites)
  - [Running the application](#Run)

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
